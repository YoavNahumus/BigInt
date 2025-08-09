#include "BigRational.hpp"
#include <stdexcept>
#include <utility>
#include "BigUint.hpp"

BigRational::BigRational() : BigRational(0) {}

BigRational::BigRational(int64_t numerator) : BigRational(numerator, 1) {}

BigRational::BigRational(BigInt numerator) : BigRational(std::move(numerator), 1) {}

BigRational::BigRational(BigInt numerator, BigUint denominator)
    : _numerator(std::move(numerator)), _denominator(std::move(denominator))
{
    if (_denominator.is_zero())
    {
        throw std::invalid_argument("Denominator cannot be zero");
    }
}

BigRational& BigRational::operator+=(const BigRational& other) &
{
    _numerator *= other._denominator;
    _numerator += other._numerator * _denominator;
    _denominator *= other._denominator;
    return *this;
}

BigRational& BigRational::operator-=(const BigRational& other) &
{
    _numerator *= other._denominator;
    _numerator -= other._numerator * _denominator;
    _denominator *= other._denominator;
    return *this;
}

BigRational& BigRational::operator*=(const BigRational& other) &
{
    _numerator *= other._numerator;
    _denominator *= other._denominator;
    return *this;
}

BigRational& BigRational::operator*=(const uint64_t number) &
{
    _numerator *= number;
    return *this;
}

BigRational& BigRational::operator/=(const BigRational& other) &
{
    _numerator *= other._denominator;
    _denominator *= other._numerator.abs();
    if (other._numerator.is_neg())
    {
        _numerator.negate();
    }
    return *this;
}

BigRational& BigRational::operator/=(const uint64_t number) &
{
    _denominator *= number;
    return *this;
}

BigRational BigRational::operator+(const BigRational& other) const
{
    BigRational temp(*this);
    temp += other;
    return temp;
}

BigRational BigRational::operator-(const BigRational& other) const
{
    BigRational temp(*this);
    temp -= other;
    return temp;
}

BigRational BigRational::operator*(const BigRational& other) const
{
    BigRational temp(*this);
    temp *= other;
    return temp;
}

BigRational BigRational::operator*(const uint64_t number) const
{
    BigRational temp(*this);
    temp *= number;
    return temp;
}

BigRational BigRational::operator/(const BigRational& other) const
{
    BigRational temp(*this);
    temp /= other;
    return temp;
}

BigRational BigRational::operator/(const uint64_t number) const
{
    BigRational temp(*this);
    temp /= number;
    return temp;
}

bool BigRational::is_zero() const
{
    return _numerator.is_zero();
}

bool BigRational::raw_equal(const BigRational& other) const
{
    return _numerator == other._numerator && _denominator == other._denominator;
}

bool BigRational::operator==(const BigRational& other) const
{
    return _numerator * other._denominator == other._numerator * _denominator;
}

bool BigRational::operator<(const BigRational& other) const
{
    return _numerator * other._denominator < other._numerator * _denominator;
}

bool BigRational::operator<=(const BigRational& other) const
{
    return _numerator * other._denominator <= other._numerator * _denominator;
}

void BigRational::minimize()
{
    const BigUint gcd = _denominator.gcd(_numerator.abs());
    _denominator /= gcd;
    _numerator /= gcd;
}