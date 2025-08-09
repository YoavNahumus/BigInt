#pragma once
#include "BigUint.hpp"

class BigInt final
{
public:
    BigInt(int64_t num = 0);
    explicit BigInt(BigUint other, bool is_negative = false);

public:
    BigInt& operator+=(const BigInt& other) &;
    BigInt& operator-=(const BigInt& other) &;
    BigInt& operator>>=(size_t bits) &;
    BigInt& operator<<=(size_t bits) &;
    BigInt& operator*=(const BigInt& other) &;
    BigInt& operator*=(const BigUint& other) &;
    BigInt& operator*=(uint64_t number) &;
    BigInt& operator/=(const BigInt& other) &;
    BigInt& operator/=(const BigUint& other) &;
    BigInt& operator%=(const BigInt& other) &;
    BigInt operator+(const BigInt& other) const;
    BigInt operator-(const BigInt& other) const;
    BigInt operator-() const;
    BigInt operator>>(size_t bits) const;
    BigInt operator<<(size_t bits) const;
    BigInt operator*(const BigInt& other) const;
    BigInt operator*(const BigUint& other) const;
    BigInt operator*(uint64_t number) const;
    BigInt operator/(const BigInt& other) const;
    BigInt operator/(const BigUint& other) const;
    BigInt operator%(const BigInt& other) const;

public:
    bool is_zero() const;
    size_t size() const;
    BigInt& negate() &;
    std::pair<BigInt, BigInt> div_and_mod(const BigInt& other) const;
    BigInt gcd(const BigInt& other) const;
    BigInt lcm(const BigInt& other) const;
    constexpr const BigUint& abs() const& { return _number; }
    constexpr bool is_neg() const { return _is_negative; }
    bool operator==(const BigInt& other) const;
    bool operator<(const BigInt& other) const;
    bool operator<=(const BigInt& other) const;
    bool operator>(const BigInt& other) const { return other < *this; }
    bool operator>=(const BigInt& other) const { return other <= *this; }

public:
    friend std::basic_istream<char>& operator>>(std::basic_istream<char>& stream, BigInt& num);
    friend std::basic_ostream<char>& operator<<(std::basic_ostream<char>& stream, const BigInt& num);

private:
    void subtract(const BigUint& other, bool opposite);

private:
    BigUint _number;
    bool _is_negative;

public:
    BigInt(const BigInt&) = default;
    BigInt& operator=(const BigInt&) = default;
    BigInt(BigInt&&) noexcept = default;
    BigInt& operator=(BigInt&&) noexcept = default;
    ~BigInt() = default;
};
