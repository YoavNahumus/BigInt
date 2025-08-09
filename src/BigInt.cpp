#include "BigInt.hpp"
#include <cstdlib>
#include <ostream>
#include <utility>
#include "BigUint.hpp"

BigInt::BigInt(int64_t num) : _number(std::abs(num)), _is_negative(num < 0) {}
BigInt::BigInt(BigUint other, bool is_negative) : _number(std::move(other)), _is_negative(is_negative) {}

BigInt& BigInt::operator+=(const BigInt& other) &
{
    if (other._is_negative == _is_negative)
    {
        _number += other._number;
        return *this;
    }

    if (other._number <= _number)
    {
        subtract(other._number, false);
        return *this;
    }

    _is_negative = !_is_negative;
    subtract(other._number, true);
    return *this;
}

BigInt& BigInt::operator-=(const BigInt& other) &
{
    if (other._is_negative != _is_negative)
    {
        _number += other._number;
        return *this;
    }

    if (other._number <= _number)
    {
        subtract(other._number, false);
        return *this;
    }

    _is_negative = !_is_negative;
    subtract(other._number, true);
    return *this;
}

BigInt& BigInt::operator>>=(size_t bits) &
{
    _number >>= bits;
    if (_number.is_zero())
    {
        _is_negative = false;
    }
    return *this;
}

BigInt& BigInt::operator<<=(size_t bits) &
{
    _number <<= bits;
    return *this;
}

BigInt& BigInt::operator*=(const BigInt& other) &
{
    _number *= other._number;
    _is_negative = _is_negative != other._is_negative;
    return *this;
}

BigInt& BigInt::operator*=(const BigUint& other) &
{
    _number *= other;
    return *this;
}

BigInt& BigInt::operator*=(uint64_t number) &
{
    _number *= number;
    return *this;
}

BigInt& BigInt::operator/=(const BigInt& other) &
{
    _number /= other._number;
    _is_negative = _is_negative != other._is_negative;
    if (_number.is_zero())
    {
        _is_negative = false;
    }

    return *this;
}

BigInt& BigInt::operator/=(const BigUint& other) &
{
    _number /= other;
    if (_number.is_zero())
    {
        _is_negative = false;
    }

    return *this;
}

BigInt& BigInt::operator%=(const BigInt& other) &
{
    _number %= other._number;
    if (_number.is_zero())
    {
        _is_negative = false;
    }

    return *this;
}

BigInt BigInt::operator+(const BigInt& other) const
{
    BigInt temp(*this);
    temp += other;
    return temp;
}

BigInt BigInt::operator-(const BigInt& other) const
{
    BigInt temp(*this);
    temp -= other;
    return temp;
}

BigInt BigInt::operator-() const
{
    BigInt temp(*this);
    temp._is_negative = !_is_negative;
    return temp;
}

BigInt BigInt::operator>>(size_t bits) const
{
    BigInt temp(*this);
    temp >>= bits;
    return temp;
}

BigInt BigInt::operator<<(size_t bits) const
{
    BigInt temp(*this);
    temp <<= bits;
    return temp;
}

BigInt BigInt::operator*(const BigInt& other) const
{
    BigInt temp(*this);
    temp *= other;
    return temp;
}

BigInt BigInt::operator*(const BigUint& other) const
{
    BigInt temp(*this);
    temp *= other;
    return temp;
}

BigInt BigInt::operator*(uint64_t number) const
{
    BigInt temp(*this);
    temp *= number;
    return temp;
}

BigInt BigInt::operator/(const BigInt& other) const
{
    BigInt temp(*this);
    temp /= other;
    return temp;
}

BigInt BigInt::operator/(const BigUint& other) const
{
    BigInt temp(*this);
    temp /= other;
    return temp;
}

BigInt BigInt::operator%(const BigInt& other) const
{
    BigInt temp(*this);
    temp %= other;
    return temp;
}

bool BigInt::is_zero() const
{
    return _number.is_zero();
}

size_t BigInt::size() const
{
    return _number.size();
}

BigInt& BigInt::negate() &
{
    _is_negative = !_is_negative;
    return *this;
}

std::pair<BigInt, BigInt> BigInt::div_and_mod(const BigInt& other) const
{
    std::pair<BigUint, BigUint> ures = _number.div_and_mod(other._number);
    return {BigInt{std::move(ures.first), _is_negative != other._is_negative},
            BigInt{std::move(ures.second), _is_negative != other._is_negative}};
}

BigInt BigInt::gcd(const BigInt& other) const
{
    return BigInt{_number.gcd(other._number), _is_negative && other._is_negative};
}

BigInt BigInt::lcm(const BigInt& other) const
{
    return BigInt{_number.lcm(other._number), _is_negative || other._is_negative};
}

bool BigInt::operator==(const BigInt& other) const
{
    return _is_negative == other._is_negative && _number == other._number;
}

bool BigInt::operator<(const BigInt& other) const
{
    if (_is_negative)
    {
        return !other._is_negative || other._number < _number;
    }

    return !other._is_negative && (_number < other._number);
}

bool BigInt::operator<=(const BigInt& other) const
{
    if (_is_negative)
    {
        return !other._is_negative || other._number <= _number;
    }

    return !other._is_negative && (_number <= other._number);
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& stream, const BigInt& num)
{
    if (num._is_negative)
    {
        stream << '-';
    }
    return stream << num._number;
}

void BigInt::subtract(const BigUint& other, bool opposite)
{
    if (opposite)
    {
        _number = other - _number;
    }
    else
    {
        _number -= other;
    }

    if (_number.is_zero())
    {
        _is_negative = false;
    }
}
