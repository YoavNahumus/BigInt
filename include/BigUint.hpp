#pragma once

#include <cfloat>
#include <cstdint>
#include <istream>
#include <vector>

class BigUint final
{
public:
    enum class Base : uint8_t
    {
        OCTAL = 010,
        DECIMAL = 10,
        HEXADECIMAL = 0x10,
    };

public:
    BigUint(uint64_t num = 0);

public:
    BigUint& operator+=(const BigUint& other) &;
    BigUint& operator-=(const BigUint& other) &;
    BigUint& operator>>=(size_t bits) &;
    BigUint& operator<<=(size_t bits) &;
    BigUint& operator*=(const BigUint& other) &;
    BigUint& operator*=(uint64_t number) &;
    BigUint& operator/=(const BigUint& other) &;
    BigUint& operator%=(const BigUint& other) &;
    BigUint operator+(const BigUint& other) const;
    BigUint operator-(const BigUint& other) const;
    BigUint operator>>(size_t bits) const;
    BigUint operator<<(size_t bits) const;
    BigUint operator*(const BigUint& other) const;
    BigUint operator*(uint64_t number) const;
    BigUint operator/(const BigUint& other) const;
    BigUint operator%(const BigUint& other) const;

public:
    bool is_zero() const;
    bool is_power_of2() const;
    size_t bit_width() const;
    size_t size() const;
    BigUint get_n_bits(size_t begin, size_t end_excluding) const;
    std::pair<BigUint, BigUint> div_and_mod(const BigUint& other) const;
    BigUint gcd(const BigUint& other) const;
    BigUint lcm(const BigUint& other) const;
    bool operator==(const BigUint& other) const;
    bool operator<(const BigUint& other) const;
    bool operator<=(const BigUint& other) const;
    bool operator>(const BigUint& other) const { return other < *this; }
    bool operator>=(const BigUint& other) const { return other <= *this; }

public:
    std::string to_string(Base base = Base::HEXADECIMAL) const;
    friend std::basic_istream<char>& operator>>(std::basic_istream<char>& stream, BigUint& num);
    friend std::basic_ostream<char>& operator<<(std::basic_ostream<char>& stream, const BigUint& num);

private:
    bool less_than_shifted(const BigUint& other, size_t shift_count);
    void fix_size();

private:
    std::vector<uint64_t> _number;

public:
    BigUint(const BigUint&) = default;
    BigUint& operator=(const BigUint&) = default;
    BigUint(BigUint&&) noexcept = default;
    BigUint& operator=(BigUint&&) noexcept = default;
    ~BigUint() = default;
};
