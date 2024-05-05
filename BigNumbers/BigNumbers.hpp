#pragma once

#include<iostream>
#include<string>

#include"DecVector.hpp"

enum class NumSpecialType : char
{
	None = 0,
	Infinity,
	Nan
};

class Integer
{
	friend class Fractional;
	friend class Math;
public:

	#pragma region Initialized

	Integer();
	template<std::signed_integral T>
	Integer(const T val)
	{
		Assign(static_cast<long long>(val));
	}
	template<std::unsigned_integral T>
	Integer(const T val)
	{
		Assign(static_cast<unsigned long long>(val));
	}
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
	bool Infinity() const;
	bool Nan() const;

	bool operator==(const Integer& other) const;
	bool operator!=(const Integer& other) const;
	bool operator<=(const Integer& other) const;
	bool operator>=(const Integer& other) const;
	bool operator<(const Integer& other) const;
	bool operator>(const Integer& other) const;

	#pragma endregion

	#pragma region Convert

	explicit operator bool() const;
	explicit operator char() const;
	explicit operator short() const;
	explicit operator int() const;
	explicit operator long long() const;
	explicit operator unsigned char() const;
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

private:
	DecVector number;
	bool negative = false;
	NumSpecialType type = NumSpecialType::None;

	void Assign(const long long num);
	void Assign(const unsigned long long num);
	void Assign(const char* num);
	void Assign(const wchar_t* num);

	bool ToBool() const;
	long long ToLLong() const;
	unsigned long long ToULLong() const;
	std::string ToString() const;

	void RemoveLastZeros();

	static Integer FabrickInfinity(bool negative);
	static Integer FabrickNan(bool negative);
};

class Fractional
{
	friend class Math;
public:

	#pragma region Initialization

	Fractional();
	template<std::signed_integral T>
	Fractional(const T val)
	{
		Assign(static_cast<long long>(val));
	}
	template<std::unsigned_integral T>
	Fractional(const T val)
	{
		Assign(static_cast<unsigned long long>(val));
	}
	template<std::floating_point T>
	Fractional(const T val)
	{
		Assign(static_cast<long double>(val));
	}
	Fractional(const char* val);
	Fractional(const wchar_t* val);
	Fractional(const Fractional& val);
	Fractional(Fractional&& val) noexcept;
	Fractional(const Integer& val);

	Fractional& operator=(const Fractional& other);

	#pragma endregion

	#pragma region Convert

	explicit operator bool() const;
	explicit operator char() const;
	explicit operator short() const;
	explicit operator int() const;
	explicit operator long long() const;
	explicit operator unsigned char() const;
	explicit operator unsigned short() const;
	explicit operator unsigned int() const;
	explicit operator unsigned long long() const;
	explicit operator float() const;
	explicit operator double() const;
	explicit operator long double() const;
	explicit operator std::string() const;
	explicit operator Integer() const;

	#pragma endregion

	#pragma region bool operators

	bool Null() const;
	bool Negative() const;
	bool Infinity() const;
	bool Nan() const;

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
	Fractional operator/(const Fractional& other) const;
	Fractional operator%(const Fractional& other) const;

	Fractional& operator+=(const Fractional& other);
	Fractional& operator-=(const Fractional& other);
	Fractional& operator*=(const Fractional& other);
	Fractional& operator/=(const Fractional& other);
	Fractional& operator%=(const Fractional& other);

	#pragma endregion

	Fractional& MadeOpposite();

	size_t AfterDot() const;
	size_t MaxSignsAfterDotDiving() const;
	Fractional& SetMaxSignsAfterDotDiving(size_t val);

	friend std::ostream& operator<<(std::ostream& out, const Fractional& val);

private:
public:
	DecVector number;
	size_t afterDot = 0;
	bool negative = false;
	NumSpecialType type = NumSpecialType::None;

	size_t maxAfterDot = 10;

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

	void EraseAfterMaxAfterDot();

	void Multi10pow(size_t count);

	static Fractional FabrickNan(bool negative);
	static Fractional FabrickInfinity(bool negative);
};

Integer operator""_b(const unsigned long long int val);
Integer operator""_b(const char* str, const size_t size);

Fractional operator""_fb(const unsigned long long int val);
Fractional operator""_fb(const char* str, const size_t size);
Fractional operator""_fb(const long double val);