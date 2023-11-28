#pragma once

#include<iostream>
#include<string>

#include"DecVector.hpp"

class Integer
{
	friend class Fractional;
public:

#pragma region Initialized

	Integer();
	Integer(const bool val);
	Integer(const short val);
	Integer(const int val);
	Integer(const long long val);
	Integer(const unsigned short val);
	Integer(const unsigned int val);
	Integer(const unsigned long long val);
	Integer(const char* val);
	Integer(const wchar_t* val);
	Integer(const Integer& other);
	Integer(Integer&& other) noexcept;
	
	Integer& operator=(const Integer& other);
	Integer& operator=(Integer&& other) noexcept;

#pragma endregion

#pragma region bool operators

	bool Null() const;
	bool Negative() const;

	bool operator==(const Integer& other) const;
	bool operator!=(const Integer& other) const;
	bool operator<=(const Integer& other) const;
	bool operator>=(const Integer& other) const;
	bool operator<(const Integer& other) const;
	bool operator>(const Integer& other) const;

#pragma endregion

#pragma region Convert

	explicit operator bool() const;
	explicit operator short() const;
	explicit operator int() const;
	explicit operator long long() const;
	explicit operator unsigned short() const;
	explicit operator unsigned int() const;
	explicit operator unsigned long long() const;
	explicit operator std::string() const;

#pragma endregion

#pragma region math operators

	Integer operator-() const;

	Integer& operator++();
	Integer& operator--();
	Integer operator++(int);
	Integer operator--(int);

	Integer operator+(const Integer& other) const;
	Integer operator-(const Integer& other) const;
	Integer operator*(const Integer& other) const;
	Integer operator/(const Integer& other) const;
	Integer operator%(const Integer& other) const;

	Integer& operator+=(const Integer& other);
	Integer& operator-=(const Integer& other);
	Integer& operator*=(const Integer& other);
	Integer& operator/=(const Integer& other);
	Integer& operator%=(const Integer& other);

#pragma endregion

	Integer& MadeOpposite();

	friend std::ostream& operator<<(std::ostream& out, const Integer& num);
	friend std::istream& operator>>(std::istream& in, Integer& num);

	friend Integer abs(Integer val);

private:
	DecVector number;
	bool negative = false;

	void Assign(const long long num);
	void Assign(const unsigned long long num);
	void Assign(const char* num);
	void Assign(const wchar_t* num);

	bool ToBool() const;
	long long ToLLong() const;
	unsigned long long ToULLong() const;
	std::string ToString() const;

	void ShiftRight();

	void RemoveLastZeros();

	Integer Div(const Integer& other, Integer& current) const;
};

class Fractional
{
public:

#pragma region Initialization

	Fractional();
	Fractional(const bool val);
	Fractional(const short val);
	Fractional(const int val);
	Fractional(const long long val);
	Fractional(const unsigned short val);
	Fractional(const unsigned int val);
	Fractional(const unsigned long long val);
	Fractional(const float val);
	Fractional(const double val);
	Fractional(const long double val);
	Fractional(const char* val);
	Fractional(const wchar_t* val);
	Fractional(const Fractional& val);
	Fractional(Fractional&& val) noexcept;
	Fractional(const Integer& val);

	Fractional& operator=(const Fractional& other);

#pragma endregion

#pragma region Convert

	explicit operator bool() const;
	explicit operator int() const;
	explicit operator short() const;
	explicit operator long long() const;
	explicit operator unsigned int() const;
	explicit operator unsigned short() const;
	explicit operator unsigned long long() const;
	explicit operator float() const;
	explicit operator double() const;
	explicit operator long double() const;
	explicit operator std::string() const;
	explicit operator Integer() const;

#pragma endregion

#pragma region bool operators

	bool operator==(const Fractional& other) const;
	bool operator!=(const Fractional& other) const;
	bool operator<(const Fractional& other) const;
	bool operator>(const Fractional& other) const;
	bool operator<=(const Fractional& other) const;
	bool operator>=(const Fractional& other) const;

#pragma endregion

#pragma region math operators

	Fractional operator-() const;

	Fractional operator+(const Fractional& other) const;
	Fractional operator-(const Fractional& other) const;
	Fractional operator*(const Fractional& other) const;


#pragma endregion

	bool Null() const;

	friend std::ostream& operator<<(std::ostream& out, const Fractional& val);

private:
	DecVector number;
	size_t afterDot = 0;
	bool negative = false;
	bool infinity = false;

	void Assign(const char* val);
	void Assign(const wchar_t* val);
	void Assign(long long val);
	void Assign(unsigned long long val);
	void Assign(long double val);

	std::string ToString() const;
	bool ToBool() const;
	long long ToLLong() const;
	unsigned long long ToULLong() const;
	long double ToLDouble() const;
	Integer ToInteger() const;

	void RemoveLastZeros();
};

Integer operator""_b(const unsigned long long int val);
Integer operator""_b(const char* str, const size_t size);

Fractional operator""_fb(const unsigned long long int val);
Fractional operator""_fb(const char* str, const size_t size);

Integer pow(const Integer& a, const Integer& b);
Integer fact(const Integer& a);
Integer abs(Integer val);

Integer min(const Integer& a, const Integer& b);
Integer max(const Integer& a, const Integer& b);