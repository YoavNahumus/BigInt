#pragma once

#include "BigInt.hpp"
#include "BigUint.hpp"

class BigRational final
{
public:
    BigRational();
    BigRational(BigInt numerator);
    explicit BigRational(BigInt numerator, BigUint denominator);

public:
    BigRational& operator+=(const BigRational& other) &;
    BigRational& operator-=(const BigRational& other) &;
    BigRational& operator*=(const BigRational& other) &;
    BigRational& operator*=(uint64_t number) &;
    BigRational& operator/=(const BigRational& other) &;
    BigRational& operator/=(uint64_t number) &;
    BigRational operator+(const BigRational& other) const;
    BigRational operator-(const BigRational& other) const;
    BigRational operator*(const BigRational& other) const;
    BigRational operator*(uint64_t number) const;
    BigRational operator/(const BigRational& other) const;
    BigRational operator/(uint64_t number) const;

public:
    bool is_zero() const;
    bool raw_equal(const BigRational& other) const;
    bool operator==(const BigRational& other) const;
    bool operator<(const BigRational& other) const;
    bool operator<=(const BigRational& other) const;
    bool operator>(const BigRational& other) const { return other < *this; }
    bool operator>=(const BigRational& other) const { return other <= *this; }

public:
    constexpr const BigInt& numerator() const { return _numerator; }
    constexpr const BigUint& denominator() const { return _denominator; }

public:
    void minimize();

private:
    BigInt _numerator;
    BigUint _denominator;
};