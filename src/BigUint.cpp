#include "BigUint.hpp"
#include <bits/floatn-common.h>
#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <ios>
#include <iostream>
#include <limits>
#include <ostream>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include "big_int.h"

union Uint128 final
{
    __uint128_t number;
    std::array<uint64_t, sizeof(__uint128_t) / sizeof(uint64_t)> parts;
};

static constexpr uint64_t BITS_IN_UINT64 = 64;

BigUint::BigUint(uint64_t num) : _number(1, num) {}

BigUint& BigUint::operator+=(const BigUint& other) &
{
    if (other._number.size() > _number.size())
    {
        _number.resize(_number.size(), 0);
    }

    const bool overflowed = big_int_add(_number.data(), _number.size(), other._number.data(), other._number.size());
    if (overflowed)
    {
        _number.push_back(1);
    }

    return *this;
}

BigUint& BigUint::operator-=(const BigUint& other) &
{
    if (other._number.size() > _number.size())
    {
        throw std::underflow_error("BigUint underflow in subtract, consider using BigInt");
    }

    const bool underflow = big_int_sub(_number.data(), _number.size(), other._number.data(), other._number.size());
    if (underflow)
    {
        throw std::underflow_error("BigUint underflow in subtract, consider using BigInt");
    }

    fix_size();
    return *this;
}

BigUint& BigUint::operator>>=(const size_t bits) &
{
    if (bits == 0)
    {
        return *this;
    }

    const size_t uint64_counts = bits / BITS_IN_UINT64;
    const size_t remainder_bits = bits % BITS_IN_UINT64;

    if (uint64_counts >= _number.size())
    {
        _number = {0};
        return *this;
    }

    std::memmove(_number.data(), &_number[uint64_counts], (_number.size() - uint64_counts) * sizeof(uint64_t));

    _number.resize(_number.size() - uint64_counts);

    const size_t non_mask_bits = BITS_IN_UINT64 - remainder_bits;
    const uint64_t mask = std::numeric_limits<uint64_t>::max() << non_mask_bits >> non_mask_bits;

    uint64_t previous_remainder = 0;
    for (uint64_t& num : std::ranges::reverse_view(_number))
    {
        const uint64_t remainder = num & mask;
        num >>= bits;
        num |= previous_remainder << non_mask_bits;
        previous_remainder = remainder;
    }

    if (_number.back() == 0 && _number.size() >= 1)
    {
        _number.pop_back();
    }

    return *this;
}

BigUint& BigUint::operator<<=(const size_t bits) &
{
    if (bits == 0)
    {
        return *this;
    }

    const size_t uint64_counts = bits / BITS_IN_UINT64;
    const size_t remainder_bits = bits % BITS_IN_UINT64;

    _number.resize(_number.size() + uint64_counts);

    std::memmove(&_number[uint64_counts], _number.data(), _number.size() * sizeof(uint64_t));
    std::memset(_number.data(), 0, uint64_counts * sizeof(uint64_t));

    const size_t non_mask_bits = BITS_IN_UINT64 - remainder_bits;
    const uint64_t mask = std::numeric_limits<uint64_t>::max() >> non_mask_bits << non_mask_bits;

    uint64_t previous_remainder = 0;
    for (uint64_t& num : _number)
    {
        const uint64_t remainder = num & mask;
        num <<= bits;
        num |= previous_remainder >> non_mask_bits;
        previous_remainder = remainder;
    }

    if (previous_remainder != 0)
    {
        _number.push_back(previous_remainder >> non_mask_bits);
    }

    return *this;
}

BigUint& BigUint::operator*=(const BigUint& other) &
{
    return operator=(std::move(*this * other));
}

BigUint& BigUint::operator*=(uint64_t number) &
{
    return operator=(std::move(*this * number));
}

BigUint& BigUint::operator/=(const BigUint& other) &
{
    return operator=(div_and_mod(other).first);
}

BigUint& BigUint::operator%=(const BigUint& other) &
{
    return operator=(div_and_mod(other).second);
}

BigUint BigUint::operator+(const BigUint& other) const
{
    BigUint temp(*this);
    temp += other;
    return temp;
}

BigUint BigUint::operator-(const BigUint& other) const
{
    BigUint temp(*this);
    temp -= other;
    return temp;
}

BigUint BigUint::operator>>(size_t bits) const
{
    BigUint temp(*this);
    temp >>= bits;
    return temp;
}

BigUint BigUint::operator<<(size_t bits) const
{
    BigUint temp(*this);
    temp <<= bits;
    return temp;
}

BigUint BigUint::operator*(const BigUint& other) const
{
    BigUint res;
    res._number.resize(_number.size() + other._number.size(), 0);

    for (size_t i = 0; i < _number.size(); ++i)
    {
        const BigUint mul = other * _number[i];
        big_int_add(&res._number[i], res._number.size() - i, mul._number.data(), mul._number.size());
    }

    res.fix_size();
    return res;
}

BigUint BigUint::operator*(uint64_t number) const
{
    BigUint low_part;
    BigUint high_part;
    const size_t size = _number.size();

    low_part._number.resize(size, 0);
    high_part._number.resize(size + 1, 0);

#pragma omp parallel for shared(size, low_part, high_part, number) default(none)
    for (uint64_t i = 0; i < size; ++i)
    {
        Uint128 mul{.number = static_cast<__uint128_t>(_number[i]) * number};
        low_part._number[i] = mul.parts[0];
        high_part._number[i + 1] = mul.parts[1];
    }

    high_part += low_part;
    return high_part;
}

BigUint BigUint::operator/(const BigUint& other) const
{
    return div_and_mod(other).first;
}

BigUint BigUint::operator%(const BigUint& other) const
{
    return div_and_mod(other).second;
}

bool BigUint::is_zero() const
{
    return _number.size() == 1 && _number[0] == 0UL;
}

bool BigUint::is_power_of2() const
{
    for (const uint64_t& value : _number)
    {
        if (value != 0)
        {
            return std::has_single_bit(value) && &value == &_number.back();
        }
    }
    return false;
}

size_t BigUint::size() const
{
    return _number.size() * sizeof(uint64_t);
}

size_t BigUint::bit_width() const
{
    return std::bit_width(_number.back()) + (_number.size() - 1) * BITS_IN_UINT64;
}

BigUint BigUint::get_n_bits(const size_t begin, const size_t end_excluding) const
{
    if (begin == end_excluding)
    {
        return 0;
    }

    BigUint res;
    res._number.resize((end_excluding - 1) / BITS_IN_UINT64 + 1, 0);
    std::memcpy(&res._number[begin / BITS_IN_UINT64], &_number[begin / BITS_IN_UINT64],
                ((end_excluding - 1) / BITS_IN_UINT64 - begin / BITS_IN_UINT64 + 1) * sizeof(uint64_t));

    const size_t remainder_low_bits = begin % BITS_IN_UINT64;
    const size_t remainder_high_bits = BITS_IN_UINT64 - end_excluding % BITS_IN_UINT64;
    res._number[begin / BITS_IN_UINT64] &= std::numeric_limits<uint64_t>::max() << remainder_low_bits;
    res._number[(end_excluding - 1) / BITS_IN_UINT64] &= std::numeric_limits<uint64_t>::max() >> remainder_high_bits;
    return res;
}

std::pair<BigUint, BigUint> BigUint::div_and_mod(const BigUint& other) const
{
    const size_t first_non_zero = other.bit_width() - 1;
    if (first_non_zero == std::numeric_limits<size_t>::max())
    {
        throw std::invalid_argument("Division by zero is undefined");
    }

    if (other.is_power_of2())
    {
        return {operator>>(first_non_zero), get_n_bits(0, first_non_zero)};
    }

    BigUint mod(*this);
    BigUint div;
    div._number.resize(_number.size(), 0);
    while (other <= mod)
    {
        const size_t bits = mod.bit_width() - 1 - first_non_zero;
        size_t uint64_counts = bits / BITS_IN_UINT64;
        size_t remainder_bits = bits % BITS_IN_UINT64;

        BigUint sub(other << remainder_bits);
        if (mod.less_than_shifted(sub, uint64_counts))
        {
            if (remainder_bits == 0)
            {
                --uint64_counts;
                remainder_bits = BITS_IN_UINT64 - 1;
                sub <<= remainder_bits;
            }
            else
            {
                --remainder_bits;
                sub >>= 1;
            }
        }

        big_int_sub(&mod._number[uint64_counts], mod._number.size() - uint64_counts, sub._number.data(),
                    sub._number.size());
        mod.fix_size();

        div._number[uint64_counts] |= 1UL << remainder_bits;
    }

    div.fix_size();

    return {div, mod};
}

BigUint BigUint::gcd(const BigUint& other) const
{
    auto [larger, smaller] =
        *this < other ? std::pair<BigUint, BigUint>{other, *this} : std::pair<BigUint, BigUint>{*this, other};

    while (!smaller.is_zero())
    {
        larger %= smaller;
        smaller._number.swap(larger._number);
    }

    return larger;
}

BigUint BigUint::lcm(const BigUint& other) const
{
    return *this * other / gcd(other);
}

bool BigUint::operator==(const BigUint& other) const
{
    if (_number.size() != other._number.size())
    {
        return false;
    }

    for (size_t i = 0; i < _number.size(); ++i)
    {
        if (_number[i] != other._number[i])
        {
            return false;
        }
    }

    return true;
}

bool BigUint::operator<(const BigUint& other) const
{
    if (_number.size() != other._number.size())
    {
        return _number.size() < other._number.size();
    }

    for (size_t i = _number.size() - 1; i < std::numeric_limits<size_t>::max(); --i)
    {
        if (_number[i] != other._number[i])
        {
            return _number[i] < other._number[i];
        }
    }

    return false;
}

bool BigUint::operator<=(const BigUint& other) const
{
    if (_number.size() != other._number.size())
    {
        return _number.size() < other._number.size();
    }

    for (size_t i = _number.size() - 1; i < std::numeric_limits<size_t>::max(); --i)
    {
        if (_number[i] != other._number[i])
        {
            return _number[i] < other._number[i];
        }
    }

    return true;
}

std::string BigUint::to_string(const Base base) const
{
    std::ostringstream oss;

    switch (base)
    {
    case Base::HEXADECIMAL:
    {
        auto iter = _number.crbegin();
        static constexpr uint32_t HEX_CHARACTERS_IN_UINT64 = sizeof(uint64_t) * 2;
        oss << *iter << std::setfill('0') << std::setw(HEX_CHARACTERS_IN_UINT64);
        while (++iter != _number.crend())
        {
            oss << *iter;
        }
        return oss.str();
    }
    default:
    {
        oss << std::hex;
        const BigUint ten_of_base = static_cast<uint8_t>(base);
        BigUint remainder(*this);
        while (remainder._number.size() > 1)
        {
            auto [div, mod] = remainder.div_and_mod(ten_of_base);
            remainder = std::move(div);
            oss << mod._number[0];
        }
        std::string easy_calc = std::to_string(remainder._number[0]);
        std::reverse(easy_calc.begin(), easy_calc.end());
        oss << easy_calc;
        std::string res = oss.str();
        std::reverse(res.begin(), res.end());
        return res;
    }
    }
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& stream, const BigUint& num)
{
    const std::ios_base::fmtflags base_flag = stream.flags() & std::ios_base::basefield;
    return stream << num.to_string(base_flag == std::ios_base::hex   ? BigUint::Base::HEXADECIMAL
                                   : base_flag == std::ios_base::oct ? BigUint::Base::OCTAL
                                                                     : BigUint::Base::DECIMAL);
}

bool BigUint::less_than_shifted(const BigUint& other, const size_t shift_count)
{
    for (size_t i = _number.size() - 1; i < std::numeric_limits<size_t>::max(); --i)
    {
        if (_number[i] != other._number[i - shift_count])
        {
            return _number[i] < other._number[i - shift_count];
        }
    }

    return false;
}

void BigUint::fix_size()
{
    size_t first_not_zero = 0;
    for (first_not_zero = _number.size(); first_not_zero > 1 && _number[first_not_zero - 1] == 0; --first_not_zero);
    _number.resize(first_not_zero);
}
