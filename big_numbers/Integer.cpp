#include "Integer.hpp"

#include<cmath>
#include<limits>
#include<algorithm>

static void multiplication(const DecVector& left, const DecVector& rigth, DecVector& res)
{
	res.clear();
	res.resize(left.size() + rigth.size() + 1);
	for (size_t i = 0; i < left.size(); i++)
	for (size_t j = 0; j < rigth.size(); j++)
	{
		res[i + j] += left[i] * rigth[j];
		if (res[i + j] > 9)
		{
			res[i + j + 1] += res[i + j] / 10;
			res[i + j] %= 10;
		}
	}
}

//static void division

#pragma region class Integer

#pragma region Initialized

Integer::Integer() : number{1} {}
Integer::Integer(const bool val)
{
	Assign(static_cast<long long>(val));
}
Integer::Integer(const short val)
{
	Assign(static_cast<long long>(val));
}
Integer::Integer(const int val)
{
	Assign(static_cast<long long>(val));
}
Integer::Integer(const long long val)
{
	Assign(static_cast<long long>(val));
}
Integer::Integer(const unsigned short val)
{
	Assign(static_cast<unsigned long long>(val));
}
Integer::Integer(const unsigned int val)
{
	Assign(static_cast<unsigned long long>(val));
}
Integer::Integer(const unsigned long long val)
{
	Assign(static_cast<unsigned long long>(val));
}
Integer::Integer(const char* val)
{
	Assign(val);
}
Integer::Integer(const wchar_t* val)
{
	Assign(val);
}
Integer::Integer(const Integer& other) : number(other.number), negative(other.negative) {}
Integer::Integer(Integer&& other) noexcept : number(other.number), negative(other.negative) {}

void Integer::Assign(long long num)
{
	negative = (num < 0);
	number.clear();
	do
	{
		number.push_back(static_cast<char>(std::abs(num % 10)));
		num /= 10;
	} while (num != 0);
}
void Integer::Assign(unsigned long long num)
{
	negative = false;
	number.clear();
	do
	{
		number.push_back(static_cast<char>(num % 10));
		num /= 10;
	} while (num != 0);
}
void Integer::Assign(const char* num)
{
	if (num == nullptr)
		return Assign(0uLL);
	negative = (num[0] == '-') ? (num++, true) : (false);
	number.clear();
	while (*num >= '0' && *num <= '9')
		number.push_back(*(num++) - '0');
	if(number.empty())
		return Assign(0uLL);
	number.reverse();
	RemoveLastZeros();
}
void Integer::Assign(const wchar_t* num)
{
	if (num == nullptr)
		return Assign(0uLL);
	negative = (num[0] == L'-') ? (num++, true) : (false);
	number.clear();
	while (*num >= L'0' && *num <= L'9')
		number.push_back(*num - L'0'), num++;
	if (number.empty())
		return Assign(0uLL);
	number.reverse();
	RemoveLastZeros();
}

Integer& Integer::operator=(const Integer& other)
{
	if (&other != this)
	{
		number = other.number;
		negative = other.negative;
	}
	return *this;
}
Integer& Integer::operator=(Integer&& other) noexcept
{
	if (&other != this)
	{
		number = other.number;
		negative = other.negative;
	}
	return *this;
}

#pragma endregion

#pragma region bool operators

bool Integer::Null() const
{
	return (number.size() == 0) || (number.size() == 1 && number[0] == '\000');
}
bool Integer::Negative() const
{
	return negative;
}

bool Integer::operator==(const Integer& other) const
{
	if (negative != other.negative)
		return false;
	if (number.size() != other.number.size())
		return false;
	for (size_t i = 0; i < number.size(); i++)
		if (number[i] != other.number[i])
			return false;
	return true;

}
bool Integer::operator!=(const Integer& other) const
{
	return !this->operator==(other);
}
bool Integer::operator<=(const Integer& other) const
{
	return this->operator<(other) || this->operator==(other);
}
bool Integer::operator>=(const Integer& other) const
{
	return this->operator>(other) || this->operator==(other);
}
bool Integer::operator<(const Integer& other) const
{
	if (negative != other.negative)
		return negative;
	if (number.size() != other.number.size())
		return (negative) ? (number.size() > other.number.size()) : (number.size() < other.number.size());
	for (size_t i = number.size(); i > 0; i--)
		if (number[i - 1] != other.number[i - 1])
			return (negative) ? (number[i - 1] > other.number[i - 1]) : (number[i - 1] < other.number[i - 1]);
	return false;
}
bool Integer::operator>(const Integer& other) const
{
	if (negative != other.negative)
		return other.negative;
	if (number.size() != other.number.size())
		return (negative) ? (number.size() < other.number.size()) : (number.size() > other.number.size());
	for (size_t i = number.size(); i > 0; i--)
		if (number[i - 1] != other.number[i - 1])
			return (negative) ? (number[i - 1] < other.number[i - 1]) : (number[i - 1] > other.number[i - 1]);
	return false;
}

#pragma endregion

#pragma region Convert

bool				Integer::ToBool() const
{
	if (number.empty())
		return false;
	if (number.size() == 1 && number[0] == 0)
		return false;
	return true;
}
long long			Integer::ToLLong() const
{
	long long res = 0;
	const size_t maxSize = static_cast<size_t>(std::log10(std::numeric_limits<long long>::max())) + 1;
	for (size_t i = 0; i < std::min(number.size(), maxSize); i++)
		res += ((negative) ? (number[i]) : (number[i])) * static_cast<size_t>(std::pow(10, i));
	return res;
}
unsigned long long	Integer::ToULLong() const
{
	unsigned long long res = 0;
	const size_t maxSize = static_cast<size_t>(std::log10(std::numeric_limits<unsigned long long>::max())) + 1;
	for (size_t i = 0; i < std::min(number.size(), maxSize); i++)
		res += number[i] * static_cast<size_t>(std::pow(10, i));
	return res;
}
std::string			Integer::ToString() const
{
	std::string res{};
	if (negative)
		res += '-';
	for (size_t i = number.size(); i > 0; i--)
		res += number[i - 1] + '0';
	//return res.empty() ? "0" : res;
	return res;
}

Integer::operator bool() const
{
	return ToBool();
}
Integer::operator short() const
{
	return static_cast<short>(ToLLong());
}
Integer::operator int() const
{
	return static_cast<int>(ToLLong());
}
Integer::operator long long() const
{
	return ToLLong();
}
Integer::operator unsigned short() const
{
	return static_cast<unsigned short>(ToULLong());
}
Integer::operator unsigned int() const
{
	return static_cast<unsigned int>(ToULLong());
}
Integer::operator unsigned long long() const
{
	return ToULLong();
}
Integer::operator std::string() const
{
	return ToString();
}

#pragma endregion

#pragma region math operators

Integer Integer::operator-() const
{
	Integer res = *this;
	res.negative = !res.negative;
	return res;
}

Integer& Integer::operator++()
{
	return this->operator+=(1);
}
Integer& Integer::operator--()
{
	return this->operator-=(1);
}
Integer Integer::operator++(int)
{
	Integer res = *this;
	this->operator+=(1);
	return res;
}
Integer Integer::operator--(int)
{
	Integer res = *this;
	this->operator-=(1);
	return res;
}

Integer Integer::operator+(const Integer& other) const
{
	Integer res = *this;
	return res += other;
}
Integer Integer::operator-(const Integer& other) const
{
	Integer res = *this;
	return res -= other;
}
Integer Integer::operator*(const Integer& other) const
{
	if (Null() || other.Null())
		return Integer{0};
	Integer res;
	multiplication(number, other.number, res.number);
	res.RemoveLastZeros();
	res.negative = negative != other.negative;
	return res;
}
Integer Integer::operator/(const Integer& other) const
{
	Integer current;
	return Div(other, current);
}
Integer Integer::operator%(const Integer& other) const
{
	Integer current;
	void(Div(other, current));
	return current;
}

Integer& Integer::operator+=(const Integer& other)
{
	if (negative && !other.negative)
		return (this->operator=(other.operator-(MadeOpposite())));
	if (other.negative)
		return this->operator-=(other.operator-());
	number.resize(number.size() + 1);
	size_t size = std::max(number.size(), other.number.size());
	for (size_t i = 0; i < size; i++)
	{
		number[i] += other.number[i];
		if (number[i] > 9)
		{
			number[i + 1]++;
			number[i] -= 10;
		}
	}
	RemoveLastZeros();
	return *this;
}
Integer& Integer::operator-=(const Integer& other)
{
	if (other.negative)
		return this->operator+=(other.operator-());
	if (negative)
		return MadeOpposite().operator+=(other).MadeOpposite();
	if (this->operator<(other))
		return this->operator=(other.operator-(*this)).MadeOpposite();
	size_t size = std::max(number.size(), other.number.size());
	for (size_t i = 0; i < size; i++)
	{
		number[i] -= other.number[i];
		if (number[i] < 0)
		{
			number[i + 1]--;;
			number[i] += 10;
			if (number.size() == (i + 1))
				number[i + 1] = '\000';
		}
	}
	RemoveLastZeros();
	return *this;
}
Integer& Integer::operator*=(const Integer& other)
{
	return this->operator=(this->operator*(other));
}
Integer& Integer::operator/=(const Integer& other)
{
	return this->operator=(this->operator/(other));
}
Integer& Integer::operator%=(const Integer& other)
{
	return this->operator=(this->operator%(other));
}

Integer Integer::Div(const Integer& other, Integer& current) const
{
	if (other.Null())
		throw "divide by zero.";
	if (Null() || abs(*this).operator<(abs(other)))
		return 0_b;
	Integer result, b = other;
	b.negative = false;
	result.number.resize(number.size());
	int x, l, r, m;
	for (size_t i = number.size(); i > 0; i--)
	{
		current.ShiftRight();
		current.number[0] = number[i - 1];
		current.RemoveLastZeros();
		x = 0, l = 0, r = 10;
		while (l <= r)
		{
			m = (l + r) / 2;
			if ((b * m) <= current)
				(x = m), (l = m + 1);
			else
				r = m - 1;
		}
		result.number[i - 1] = x;
		current = current - b * x;
	}
	result.negative = negative != other.negative && !result.Null();
	result.RemoveLastZeros();
	return result;
}




//Integer Integer::Div(const Integer& other, Integer& current) const
//{
//	if (other.Null())
//		throw "divide by zero.";
//	if (Null() || abs(*this).operator<(abs(other)))
//		return 0_b;
//	Integer result, t, b = other;
//	b.negative = false;
//	result.number.resize(number.size());
//	int x, l, r, m;
//	for (size_t i = number.size(); i > 0; i--)
//	{
//		current.ShiftRight();
//		current.number[0] = number[i - 1];
//		current.RemoveLastZeros();
//		x = 0, l = 0, r = 10;
//		while (l <= r)
//		{
//			m = (l + r) / 2;
//			t = b * m;
//			if (t <= current)
//				(x = m), (l = m + 1);
//			else
//				r = m - 1;
//		}
//		result.number[i - 1] = x;
//		current = current - b * x;
//	}
//	result.negative = negative != other.negative && !result.Null();
//	result.RemoveLastZeros();
//	return result;
//}

#pragma endregion

std::ostream& operator<<(std::ostream& out, const Integer& num)
{
	return out << num.ToString();
}
std::istream& operator>>(std::istream& in, Integer& num)
{
	num.number.clear();
	char pr = in.get();
	num.negative = (pr == '-');
	if ((pr < '0' || pr > '9') && (pr != '-'))
		return (num = 0_b), in;
	else if (pr >= '0' && pr <= '9')
		num.number.push_back(pr - '0');
	pr = in.get();
	while (pr >= '0' && pr <= '9')
	{
		num.number.push_back(pr - '0');
		pr = in.get();
	}
	num.number.reverse();
	while (num.number.back() == '\000' && num.number.size() != 1)
		num.number.pop_back();
	if (num.Null())
		num.negative = false;
	return in;
}

Integer& Integer::MadeOpposite()
{
	negative = !negative;
	return *this;
}

void Integer::ShiftRight()
{
	if (number.size() == 0)
		return number.push_back(0);
	number.push_back(number.back());
	for (size_t i = number.size() - 2; i > 0; --i) number[i] = number[i - 1];
	number[0] = 0;
}

void Integer::RemoveLastZeros()
{
	while (number.back() == '\000' && number.size() != 1)
		number.pop_back();
}

Integer operator""_b(const unsigned long long int val)
{
	return Integer{ val };
}
Integer operator""_b(const char* str, const size_t size)
{
	return Integer{ str };
}

#pragma endregion

#pragma region class Fractional

#pragma region Initialization

Fractional::Fractional() {}
Fractional::Fractional(const bool val) 
{
	Assign(static_cast<long long>(val));
}
Fractional::Fractional(const short val) 
{
	Assign(static_cast<long long>(val));
}
Fractional::Fractional(const int val) 
{
	Assign(static_cast<long long>(val));
}
Fractional::Fractional(const long long val) 
{
	Assign(val);
}
Fractional::Fractional(const unsigned short val) 
{
	Assign(static_cast<unsigned long long>(val));
}
Fractional::Fractional(const unsigned int val) 
{
	Assign(static_cast<unsigned long long>(val));
}
Fractional::Fractional(const unsigned long long val) 
{
	Assign(val);
}
Fractional::Fractional(const float val) 
{
	Assign(static_cast<long double>(val));
}
Fractional::Fractional(const double val) 
{
	Assign(static_cast<long double>(val));
}
Fractional::Fractional(const long double val) 
{
	Assign(val);
}
Fractional::Fractional(const char* val) 
{
	Assign(val);
}
Fractional::Fractional(const wchar_t* val) 
{
	Assign(val);
}
Fractional::Fractional(const Fractional& val) :
	number{ val.number }, afterDot{ val.afterDot }, negative{ val.negative }, infinity{val.infinity} {}
Fractional::Fractional(Fractional&& val) noexcept :
	number{ val.number }, afterDot{ val.afterDot }, negative{ val.negative }, infinity{ val.infinity } {}
Fractional::Fractional(const Integer& val) :
	number{ val.number }, negative{ val.negative }, infinity{ false }, afterDot{0} {}

void Fractional::Assign(const char* num)
{
	if (num == nullptr)
		return Assign(0uLL);
	negative = (num[0] == '-') ? (num++, true) : (false);
	number.clear();
	if (num[0] == 'i' && num[1] == 'n' && num[2] == 'f')
		return (infinity = true), void();
	while (*num >= '0' && *num <= '9')
		number.push_back(*(num++) - '0');
	afterDot = 0uLL;
	if (*num == '.' || *num == ',')
	{
		num++;
		while (*num >= '0' && *num <= '9')
			number.push_back(*(num++) - '0'), afterDot++;
	}
	if (number.empty())
		return Assign(0uLL);
	number.reverse();
	RemoveLastZeros();
}
void Fractional::Assign(const wchar_t* num)
{
	if (num == nullptr)
		return Assign(0uLL);
	negative = (num[0] == '-') ? (num++, true) : (false);
	number.clear();
	if (num[0] == L'i' && num[1] == L'n' && num[2] == L'f')
		return (infinity = true), void();
	while (*num >= L'0' && *num <= L'9')
		number.push_back(*(num++) - L'0');
	afterDot = 0uLL;
	if (*num == L'.' || *num == L',')
	{
		num++;
		while (*num >= L'0' && *num <= L'9')
			number.push_back(*(num++) - L'0'), afterDot++;
	}
	if (number.empty())
		return Assign(0uLL);
	number.reverse();
	RemoveLastZeros();
}
void Fractional::Assign(long long num)
{
	negative = (num < 0LL);
	infinity = false;
	number.clear();
	do
	{
		number.push_back(static_cast<char>(std::abs(num % 10)));
		num /= 10;
	} while (num != 0);
}
void Fractional::Assign(unsigned long long num)
{
	negative = infinity = false;
	number.clear();
	do
	{
		number.push_back(static_cast<char>(num % 10));
		num /= 10;
	} while (num != 0);
}
void Fractional::Assign(long double val)
{
	infinity = isinf(val);
	negative = (val < 0.l);
	number.clear();
	if (infinity)
		return;

	long long tail = std::abs(static_cast<long long>(modfl(val, &val) * 10000000000000000.l));
	while (tail % 10 == 0 && tail != 0)tail /= 10;
	while (tail != 0)
	{
		number.push_back(static_cast<char>(tail % 10));
		tail /= 10;
	}
	afterDot = number.size();
	do
	{
		number.push_back(abs(static_cast<char>(fmodl(val, 10.l))));
		(void)modfl(val / 10.f, &val);
	} while (val != 0.l);
}

Fractional& Fractional::operator=(const Fractional& other)
{
	if (this == &other)
		return *this;
	negative = other.negative;
	infinity = other.infinity;
	afterDot = other.afterDot;
	number = infinity ? DecVector{} : other.number;
	return *this;
}

#pragma endregion

#pragma region Convert

Fractional::operator bool() const
{
	return ToBool();
}
Fractional::operator short() const
{
	return static_cast<short>(ToLLong());
}
Fractional::operator int() const
{
	return static_cast<int>(ToLLong());
}
Fractional::operator long long() const
{
	return ToLLong();
}
Fractional::operator unsigned short() const
{
	return static_cast<unsigned short>(ToULLong());
}
Fractional::operator unsigned int() const
{
	return static_cast<unsigned int>(ToULLong());
}
Fractional::operator unsigned long long() const
{
	return ToULLong();
}
Fractional::operator float() const
{
	return static_cast<float>(ToLDouble());
}
Fractional::operator double() const
{
	return static_cast<double>(ToLDouble());
}
Fractional::operator long double() const
{
	return ToLDouble();
}
Fractional::operator std::string() const
{
	return ToString();
}
Fractional::operator Integer() const
{
	return ToInteger();
}

std::string Fractional::ToString() const
{
	std::string res = "";
	if (infinity)
		return (negative ? ("-inf") : ("inf"));;
	for (size_t i = 0; i < afterDot; i++)
		res += number[i] + '0';
	if (afterDot != 0)
		res += '.';
	for (size_t i = afterDot; i < number.size(); i++)
		res += number[i] + '0';
	if (negative)
		res += '-';
	for (size_t i = 0; i < (res.size() / 2); i++)
		std::swap(res[i], res[res.size() - i - 1]);
	return res;
}
bool Fractional::ToBool() const
{
	return !((number.size() - afterDot < 2) && (number[afterDot] == '\000') && !infinity);
}
long long Fractional::ToLLong() const
{
	if (infinity)
		return negative ? std::numeric_limits<long long>::min() : std::numeric_limits<long long>::max();
	long long res = 0;
	const size_t maxSize = static_cast<size_t>(std::log10(std::numeric_limits<long long>::max())) + 1;
	for (size_t i = 0; i < std::min(number.size() - afterDot, maxSize); i++)
		res += ((negative) ? (-number[i + afterDot]) : (number[i + afterDot])) * static_cast<long long>(std::pow(10, i));
	return res;
}
unsigned long long Fractional::ToULLong() const
{
	if (infinity)
		return negative ? std::numeric_limits<unsigned long long>::min() : std::numeric_limits<unsigned long long>::max();
	unsigned long long res = 0;
	const size_t maxSize = static_cast<size_t>(std::log10(std::numeric_limits<long long>::max()));
	for (size_t i = 0; i < std::min(number.size() - afterDot, maxSize); i++)
		res += number[i + afterDot] * static_cast<size_t>(std::pow(10, i));
	return res;
}
long double Fractional::ToLDouble() const
{
	if (infinity)
		return negative ? (-1.l / sin(0)) : (1.l / sin(0));
	long double res = 0.l;
	const size_t maxSize = static_cast<size_t>(std::log10(std::numeric_limits<long double>::max())) + 1;
	for (size_t i = 0; i < std::min(number.size() - afterDot, maxSize); i++)
		res += ((negative) ? (-number[i + afterDot]) : (number[i + afterDot])) * std::pow(10, i);
	for (size_t i = afterDot; i > 0; i--)
		res += ((negative) ? (-number[i - 1]) : (number[i - 1])) / std::pow(10, afterDot - i + 1);
	return res;
}
Integer Fractional::ToInteger() const
{
	Integer res;
	res.number.clear();
	for (size_t i = afterDot; i < number.size(); i++)
		res.number.push_back(number[i]);
	res.negative = negative;
	return res;
}

#pragma endregion

#pragma region bool operators

bool Fractional::operator==(const Fractional& other) const
{
	if (negative != other.negative || infinity != other.infinity || afterDot != other.afterDot || number.size() != other.number.size())
		return false;
	if (infinity && other.infinity && (negative == other.negative))
		return true;
	for (size_t i = 0; i < number.size(); i++)
		if (number[i] != other.number[i])
			return false;
	return true;
}
bool Fractional::operator!=(const Fractional& other) const
{
	return !this->operator==(other);
}
bool Fractional::operator<(const Fractional& other) const
{
	if (negative != other.negative)
		return negative;
	if (infinity)
		return (other.infinity) ? false : negative;
	if ((number.size() - afterDot) != (other.number.size() - other.afterDot))
		return (number.size() - afterDot) > (other.number.size() - other.afterDot) ? (negative) : (!negative);
	for (size_t i = 0; i < number.size() - afterDot; i++)
		if (number[i + afterDot] != other.number[i + other.afterDot])
			return negative ? (number[i + afterDot] > other.number[i + other.afterDot]) : (number[i + afterDot] < other.number[i + other.afterDot]);
	for (size_t i = 0; i < std::min(afterDot, other.afterDot); i++)
		if (number[afterDot - i - 1] != other.number[other.afterDot - i - 1])
			return negative ? (number[afterDot - i - 1] > other.number[other.afterDot - i - 1]) : (number[afterDot - i - 1] < other.number[other.afterDot - i - 1]);
	if (afterDot != other.afterDot)
	{
		if (afterDot < other.afterDot)
			return !negative;
		if (afterDot > other.afterDot)
			return negative;
	}
	return false;
}
bool Fractional::operator>(const Fractional& other) const
{
	if (negative != other.negative)
		return !negative;
	if (infinity)
		return (other.infinity) ? false : !negative;
	if ((number.size() - afterDot) != (other.number.size() - other.afterDot))
		return (number.size() - afterDot) < (other.number.size() - other.afterDot) ? (negative) : (!negative);
	for (size_t i = 0; i < number.size() - afterDot; i++)
		if (number[i + afterDot] != other.number[i + other.afterDot])
			return negative ? (number[i + afterDot] < other.number[i + other.afterDot]) : (number[i + afterDot] > other.number[i + other.afterDot]);
	for (size_t i = 0; i < std::min(afterDot, other.afterDot); i++)
		if (number[afterDot - i - 1] != other.number[other.afterDot - i - 1])
			return negative ? (number[afterDot - i - 1] < other.number[other.afterDot - i - 1]) : (number[afterDot - i - 1] > other.number[other.afterDot - i - 1]);
	if (afterDot != other.afterDot)
	{
		if (afterDot > other.afterDot)
			return !negative;
		if (afterDot < other.afterDot)
			return negative;
	}
	return false;
}
bool Fractional::operator<=(const Fractional& other) const
{
	return !this->operator>(other);
}
bool Fractional::operator>=(const Fractional& other) const
{
	return !this->operator<(other);
}

#pragma endregion

Fractional Fractional::operator-() const
{
	auto res = *this;
	res.negative = !negative;
	return res;
}

Fractional Fractional::operator+(const Fractional& other) const
{
	if (negative)
		return  (other.negative) ? (this->operator-().operator+(other.operator-()).operator-()) : (other.operator-(this->operator-()));
	if (other.negative)
		return this->operator-(other.operator-());
	Fractional res = *this;
	size_t lP = (afterDot > other.afterDot) ? (afterDot - other.afterDot) : (0);
	size_t rP = (afterDot > other.afterDot) ? (0) : (other.afterDot - afterDot);
	res.number.resize(res.number.size() + 1);
	for (size_t i = 0; i < std::max(res.number.size(), other.number.size()); i++)
	{
		res.number[i + lP] += other.number[i + rP];
		if (res.number[i + lP] > 9)
		{
			res.number[i + 1 + lP]++;
			res.number[i + lP] -= 10;
		}
	}
	if (rP != 0)
		for (size_t i = rP; i > 0; i--)
			res.number.push_begin(other.number[i - 1]), res.afterDot++;
	res.RemoveLastZeros();
	return res;
}
Fractional Fractional::operator-(const Fractional& other) const
{
	if (other.negative) return this->operator+(other.operator-());
	if (negative) return this->operator-().operator+(other).operator-();
	if (this->operator<(other)) return (other.operator-(*this)).operator-();
	Fractional res = *this;
	if (res.afterDot < other.afterDot)
	{
		res.number.push_begin_memory(other.afterDot - res.afterDot);
		res.afterDot += other.afterDot - res.afterDot;
	}
	size_t lP = (res.afterDot > other.afterDot) ? (res.afterDot - other.afterDot) : (0);
	for (size_t i = 0; i < std::max(res.number.size(), other.number.size()); i++)
	{
		res.number[i + lP] -= other.number[i];
		if (res.number[i + lP] < 0)
		{
			res.number[i + 1 + lP]--;
			res.number[i + lP] += 10;
			if (res.number.size() == i + 1)
				res.number[i + 1 + lP] = '\000';
		}
	}
	res.RemoveLastZeros();
	return res;
}
Fractional Fractional::operator*(const Fractional& other) const
{
	if (infinity)
		return *this;
	if (other.infinity)
		return other;
	if (Null() || other.Null())
		return Fractional{};
	Fractional res;
	multiplication(number, other.number, res.number);
	res.afterDot = afterDot + other.afterDot;
	res.RemoveLastZeros();
	res.negative = negative != other.negative && !res.Null();
	return res;
}

bool Fractional::Null() const
{
	return (infinity) ? (false) : ((afterDot == 0) && (number.size() < 2) && (number[0] == '\000'));
}

void Fractional::RemoveLastZeros()
{
	size_t i = 0;
	while (number[i] == '\000' && afterDot != 0)
		i++, afterDot--;
	number.erase(0, i);
	while (number.back() == '\000' && (number.size() - afterDot) > 1)
		number.pop_back();
}

std::ostream& operator<<(std::ostream& out, const Fractional& val)
{
	return out << val.ToString();
}

Fractional operator""_fb(const unsigned long long int val)
{
	return Fractional{ val };
}
Fractional operator""_fb(const char* str, const size_t size)
{
	return Fractional{ str };
}

#pragma endregion

#pragma region math

Integer pow(const Integer& a, const Integer& b)
{
	if (b.Negative())
		return 0_b;
	Integer res = 1;
	for (Integer i = 0_b; i < b; i++)
		res *= a;
	return res;
}
Integer fact(const Integer& a)
{
	if (a.Negative() || a.Null())
		return 0_b;
	Integer res = 1;
	for (Integer i = 1_b; i <= a; i++)
		res *= i;
	return res;
}
Integer abs(Integer val)
{
	val.negative = false;
	return val;
}

Integer min(const Integer& a, const Integer& b)
{
	return (a < b) ? a : b;
}
Integer max(const Integer& a, const Integer& b)
{
	return (a > b) ? a : b;
}

#pragma endregion