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

namespace division
{
	static bool less(const DecVector& left, const DecVector& right)
	{
		if (left.size() != right.size())
			return left.size() < right.size();
		for (size_t i = left.size(); i > 0; --i)
			if (left[i - 1] != right[i - 1])
				return left[i - 1] < right[i - 1];
		return false;
	}
	static bool more(const DecVector& left, const DecVector& right)
	{
		if (left.size() != right.size())
			return left.size() > right.size();
		for (size_t i = left.size(); i > 0; --i)
			if (left[i - 1] != right[i - 1])
				return left[i - 1] > right[i - 1];
		return false;
	}

	static void smal_multiplication(DecVector& a, const char s)
	{
		a.resize(a.size() + 1);
		char plus = 0;
		for (size_t i = 0; i < a.size(); i++)
		{
			a[i] = a[i] * s + plus;
			plus = 0;
			if (a[i] > 9)
			{
				plus = a[i] / 10;
				a[i] %= 10;
			}
		}
		while (a.back() == '\000')
			a.pop_back();
	}

	static void fill_pr(DecVector& pr, const DecVector& left, const DecVector& right)
	{
		pr.clear();
		for (size_t i = 1; i <= right.size() && (left.size() - i) < left.size() ; i++)
			pr.push_back(left[left.size() - i]);
		pr.reverse();
		if (less(pr, right) && left.size() > right.size() && pr.back() != '\000')
			pr.push_begin(left[left.size() - right.size() - 1]);
	}

	static void find_pr_mul(DecVector& pr_mul, char& prC, const DecVector& pr, const DecVector& right)
	{
		for (prC = 9; prC >= 0; prC--)
		{
			pr_mul = right;
			smal_multiplication(pr_mul, prC);
			if (!more(pr_mul, pr))
				break;
		}
	}

	static void minus_equal(DecVector& left, const DecVector& right)
	{
		for (size_t i = 0; i < std::min(left.size(), right.size()); i++)
		{
			left[i] -= right[i];
			if (left[i] < '\000')
			{
				--left[i + 1];
				left[i] += 10;
			}
		}
		while (left.back() == '\000')
			left.pop_back();
	}

	static void division(const DecVector& left, const DecVector& right, DecVector& res, DecVector& pr)
	{
		char prC;
		size_t i = 0;
		DecVector pr_mul;
		fill_pr(pr, left, right);
		i += pr.size();
		res.clear();
		while (i <= left.size())
		{
			find_pr_mul(pr_mul, prC, pr, right);
			res.push_back(prC);
			minus_equal(pr, pr_mul);
			if(i != left.size())
				pr.push_begin(left[left.size() - i - 1]);	
			++i;
		}
		res.reverse();
	}

	static void division(const DecVector& left, const DecVector& right, DecVector& res)
	{
		DecVector pr;
		division(left, right, res, pr);
	}
}

#pragma region class Integer

#pragma region Initialized

Integer::Integer() : number{1} {}
Integer::Integer(const char* val)
{
	Assign(val);
}
Integer::Integer(const wchar_t* val)
{
	Assign(val);
}
Integer::Integer(const Integer& other) :
	number(other.number), negative(other.negative), infinity(other.infinity), nan(other.nan) {}
Integer::Integer(Integer&& other) noexcept :
	number(other.number), negative(other.negative), infinity(other.infinity), nan(other.nan) {}

void Integer::Assign(long long num)
{
	negative = (num < 0);
	nan = infinity = false;
	number.clear();
	do
	{
		number.push_back(static_cast<char>(std::abs(num % 10)));
		num /= 10;
	} while (num != 0);
}
void Integer::Assign(unsigned long long num)
{
	negative = nan = infinity = false;
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
	nan = infinity = false;
	negative = (num[0] == '-') ? (num++, true) : (false);
	number.clear();
	if (num[0] == 'i' && num[1] == 'n' && num[2] == 'f')
		return (infinity = true), void();
	if (num[0] == 'n' && num[1] == 'a' && num[2] == 'n')
		return (nan = true), void();
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
	nan = infinity = false;
	negative = (num[0] == '-') ? (num++, true) : (false);
	number.clear();
	if (num[0] == L'i' && num[1] == L'n' && num[2] == L'f')
		return (infinity = true), void();
	if (num[0] == L'n' && num[1] == L'a' && num[2] == L'n')
		return (nan = true), void();
	while (*num >= L'0' && *num <= L'9')
		number.push_back(*(num++) - L'0');
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
		infinity = other.infinity;
		nan = other.nan;
	}
	return *this;
}
Integer& Integer::operator=(Integer&& other) noexcept
{
	if (&other != this)
	{
		number = other.number;
		negative = other.negative;
		infinity = other.infinity;
		nan = other.nan;
	}
	return *this;
}

#pragma endregion

#pragma region bool operators

bool Integer::Null() const
{
	return ((number.empty()) || (number.size() == 1 && number[0] == '\000')) && !(nan || infinity);
}
bool Integer::Negative() const
{
	return negative;
}
bool Integer::Infinity() const
{
	return infinity;
}
bool Integer::Nan() const
{
	return nan;
}

bool Integer::operator==(const Integer& other) const
{
	if ((negative != other.negative) || (infinity != other.infinity) || (nan || other.nan) || number.size() != other.number.size())
		return false;
	if (infinity && other.infinity)
		return true;
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
	return !(this->operator>(other));
}
bool Integer::operator>=(const Integer& other) const
{
	return !(this->operator<(other));
}
bool Integer::operator<(const Integer& other) const
{
	if (nan || other.nan)
		return (negative == other.negative) ? (false) : (negative);
	if (negative != other.negative)
		return negative;
	if (infinity || other.infinity)
	{
		if (infinity && other.infinity)
			return false;
		return negative ? (infinity) : (!infinity);
	}
	if (number.size() != other.number.size())
		return (negative) ? (number.size() > other.number.size()) : (number.size() < other.number.size());
	for (size_t i = number.size(); i > 0; i--)
		if (number[i - 1] != other.number[i - 1])
			return (negative) ? (number[i - 1] > other.number[i - 1]) : (number[i - 1] < other.number[i - 1]);
	return false;
}
bool Integer::operator>(const Integer& other) const
{
	if (nan || other.nan)
		return (negative == other.negative) ? (false) : (other.negative);
	if (negative != other.negative)
		return other.negative;
	if (infinity || other.infinity)
	{
		if (infinity && other.infinity)
			return false;
		return negative ? (other.infinity) : (!other.infinity);
	}
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
	return !Null();
}
long long			Integer::ToLLong() const
{
	long long res = 0;
	if (nan)
		return res;
	if (infinity)
		return negative ? std::numeric_limits<long long>::min() : std::numeric_limits<long long>::max();
	const size_t maxSize = static_cast<size_t>(std::log10(std::numeric_limits<long long>::max())) + 1;
	for (size_t i = 0; i < std::min(number.size(), maxSize); i++)
		res += ((negative) ? (-number[i]) : (number[i])) * static_cast<size_t>(std::pow(10, i));
	return res;
}
unsigned long long	Integer::ToULLong() const
{
	unsigned long long res = 0;
	if (nan)
		return res;
	if (infinity)
		return negative ? 0uLL : std::numeric_limits<unsigned long long>::max();
	const size_t maxSize = static_cast<size_t>(std::log10(std::numeric_limits<unsigned long long>::max())) + 1;
	for (size_t i = 0; i < std::min(number.size(), maxSize); i++)
		res += number[i] * static_cast<size_t>(std::pow(10, i));
	return res;
}
std::string			Integer::ToString() const
{
	std::string res = "";
	if (negative)
		res += '-';
	if (nan)
		return res + "nan";
	if (infinity)
		return res + "inf";
	for (size_t i = number.size(); i > 0; i--)
		res += number[i - 1] + '0';
	return res;
}

Integer::operator bool() const
{
	return ToBool();
}
Integer::operator char() const
{
	return static_cast<char>(ToLLong());
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
Integer::operator unsigned char() const
{
	return static_cast<unsigned char>(ToULLong());
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
	return res.MadeOpposite();
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
	if (nan || other.nan)
		return FabrickNan(negative != other.negative);
	if (infinity || other.infinity)
		return (Null() || other.Null()) ? (FabrickNan(negative != other.negative)) : (FabrickInfinity(negative != other.negative));
	if (Null() || other.Null())
		return Integer{};
	Integer res;
	multiplication(number, other.number, res.number);
	res.RemoveLastZeros();
	res.negative = negative != other.negative;
	return res;
}
Integer Integer::operator/(const Integer& other) const
{
	Integer res = *this;
	return res /= other;
}
Integer Integer::operator%(const Integer& other) const
{
	Integer res = *this;
	return res %= other;
}

Integer& Integer::operator+=(const Integer& other)
{
	if (nan || other.nan)
		return this->operator=(FabrickNan(negative));
	if (infinity || other.infinity)
	{
		if (infinity && other.infinity)
			return (negative != other.negative) ? this->operator=(Integer{}) : this->operator=(FabrickInfinity(negative));
		return this->operator=(FabrickInfinity(infinity ? negative : other.negative));
	}
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
	if (nan || other.nan)
		return this->operator=(FabrickNan(negative));
	if (infinity || other.infinity)
	{
		if (infinity && other.infinity)
			return (negative == other.negative) ? (this->operator=(Integer{})) : (this->operator=(FabrickInfinity(negative)));
		return this->operator=(FabrickInfinity(infinity ? negative : other.negative));
	}
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
	if (nan || other.nan)
		return this->operator=(FabrickNan(negative != other.negative));
	if (other.Null())
		return (Null() || infinity) ? this->operator=(FabrickNan(negative != other.negative)) : this->operator=(FabrickInfinity(negative != other.negative));
	if (infinity || other.infinity)
	{
		if (infinity && other.infinity)
		{
			Integer res{ 1 };
			res.negative = negative != other.negative;
			return this->operator=(res);
		}
		if (infinity)
		{
			negative = negative != other.negative;
			return *this;
		}
		Assign(0uLL);
		return *this;
	}
	if (Null())
	{
		negative = false;
		return *this;
	}
	DecVector res;
	division::division(number,other.number, res);
	number = res;
	negative = negative != other.negative;
	return *this;
}
Integer& Integer::operator%=(const Integer& other)
{
	if (nan || other.nan || infinity || other.infinity || other.Null())
		return this->operator=(FabrickNan(false));
	if (Null())
	{
		negative = false;
		return *this;
	}
	DecVector res, pr;
	division::division(number, other.number, res, pr);
	number = pr;
	if (number.empty())
		number.push_back('\000');
	negative = false;
	return *this;
}

#pragma endregion

std::ostream& operator<<(std::ostream& out, const Integer& num)
{
	return out << num.ToString();
}
std::istream& operator>>(std::istream& in, Integer& num)
{
	num.number.clear();
	num.nan = num.infinity = false;
	char pr = in.get();
	num.negative = (pr == '-');
	if ((pr < '0' || pr > '9') && (pr != '-'))
		return (num.Assign(0uLL)), in;
	else if (pr >= '0' && pr <= '9')
		num.number.push_back(pr - '0');
	pr = in.get();
	if (pr == 'i')
	{
		if (in.get() == 'n')
			if (in.get() == 'f')
				num = Integer::FabrickInfinity(num.negative);
		return in;
	}
	else if (pr == 'n')
	{
		if (in.get() == 'a')
			if (in.get() == 'n')
				num = Integer::FabrickNan(num.negative);
		return in;
	}
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
	negative = Null() ? false : !negative;
	return *this;
}

void Integer::RemoveLastZeros()
{
	while (number.back() == '\000' && number.size() > 1uLL)
		number.pop_back();
}

Integer Integer::FabrickInfinity(bool negative)
{
	Integer res;
	res.infinity = true;
	res.negative = negative;
	return res;
}
Integer Integer::FabrickNan(bool negative)
{
	Integer res;
	res.nan = true;
	res.negative = negative;
	return res;
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
Fractional::Fractional(const char* val) 
{
	Assign(val);
}
Fractional::Fractional(const wchar_t* val) 
{
	Assign(val);
}
Fractional::Fractional(const Fractional& val) :
	number{ val.number }, afterDot{ val.afterDot }, negative{ val.negative }, infinity{ val.infinity }, maxAfterDot{val.maxAfterDot} {}
Fractional::Fractional(Fractional&& val) noexcept :
	number{ val.number }, afterDot{ val.afterDot }, negative{ val.negative }, infinity{ val.infinity }, maxAfterDot{ val.maxAfterDot } {}
Fractional::Fractional(const Integer& val) :
	number{ val.number }, negative{ val.negative }, infinity{ false }, afterDot{0} {}

void Fractional::Assign(const char* num)
{
	if (num == nullptr)
		return Assign(0uLL);
	infinity = nan = false;
	negative = (num[0] == '-') ? (num++, true) : (false);
	number.clear();
	if (num[0] == 'i' && num[1] == 'n' && num[2] == 'f')
		return (infinity = true), void();
	if (num[0] == 'n' && num[1] == 'a' && num[2] == 'n')
		return (nan = true), void();
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
	infinity = nan = false;
	negative = (num[0] == L'-') ? (num++, true) : (false);
	number.clear();
	if (num[0] == L'i' && num[1] == L'n' && num[2] == L'f')
		return (infinity = true), void();
	if (num[0] == L'n' && num[1] == L'a' && num[2] == L'n')
		return (nan = true), void();
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
	infinity = nan = false;
	afterDot = 0uLL;
	number.clear();
	do
	{
		number.push_back(static_cast<char>(std::abs(num % 10)));
		num /= 10;
	} while (num != 0);
}
void Fractional::Assign(unsigned long long num)
{
	negative = infinity = nan = false;
	afterDot = 0uLL;
	number.clear();
	do
	{
		number.push_back(static_cast<char>(num % 10));
		num /= 10;
	} while (num != 0);
}
void Fractional::Assign(long double val)
{
	negative = (val < 0.l);
	afterDot = nan = infinity = false;
	number.clear();
	if (isnan(val))
	{
		nan = true;
		number.push_back('\000');
		return;
	}
	if (isinf(val))
	{
		infinity = true;
		number.push_back('\000');
		return;
	}
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
	nan = other.nan;
	maxAfterDot = other.maxAfterDot;
	number = (infinity || nan) ? DecVector{} : other.number;
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

std::string			Fractional::ToString()	const
{
	std::string res;
	if (infinity)
		return negative ? "-inf" : "inf";
	if (nan)
		return negative ? "-nan" : "nan";
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
bool				Fractional::ToBool()	const
{
	return !Null();
}
long long			Fractional::ToLLong()	const
{
	if (infinity)
		return negative ? std::numeric_limits<long long>::min() : std::numeric_limits<long long>::max();
	if (nan)
		return long long{};
	long long res = 0;
	const size_t maxSize = static_cast<size_t>(std::log10(std::numeric_limits<long long>::max())) + 1;
	for (size_t i = 0; i < std::min(number.size() - afterDot, maxSize); i++)
		res += negative ? -number[i + afterDot] : number[i + afterDot] * static_cast<long long>(std::pow(10, i));
	return res;
}
unsigned long long	Fractional::ToULLong()	const
{
	if (infinity)
		return negative ? std::numeric_limits<unsigned long long>::min() : std::numeric_limits<unsigned long long>::max();
	if (nan)
		return unsigned long long{};
	unsigned long long res = 0;
	const size_t maxSize = static_cast<size_t>(std::log10(std::numeric_limits<long long>::max()));
	for (size_t i = 0; i < std::min(number.size() - afterDot, maxSize); i++)
		res += number[i + afterDot] * static_cast<size_t>(std::pow(10, i));
	return res;
}
long double			Fractional::ToLDouble() const
{
	if (infinity)
		return negative ? (-1.l / sin(0)) : (1.l / sin(0));
	if (nan)
		return negative ? (-1.l / sin(0) * 0.l) : (1.l / sin(0) * 0.l);
	long double res = 0.l;
	const size_t maxSize = static_cast<size_t>(std::log10(std::numeric_limits<long double>::max())) + 1;
	for (size_t i = 0; i < std::min(number.size() - afterDot, maxSize); i++)
		res += negative ? -number[i + afterDot] : number[i + afterDot] * std::pow(10, i);
	for (size_t i = afterDot; i > 0; i--)
		res += negative ? -number[i - 1] : number[i - 1] / std::pow(10, afterDot - i + 1);
	return res;
}
Integer				Fractional::ToInteger() const
{
	Integer res;
	res.number.clear();
	for (size_t i = afterDot; i < number.size(); i++)
		res.number.push_back(number[i]);
	res.negative = negative;
	res.infinity = infinity;
	res.nan = nan;
	return res;
}

#pragma endregion

#pragma region bool operators

bool Fractional::Null() const
{
	return ((number.empty()) || (number.size() == 1 && number[0] == '\000')) && !(nan || infinity || afterDot != 0);
}
bool Fractional::Negative() const
{
	return negative;
}
bool Fractional::Infinity() const
{
	return infinity;
}
bool Fractional::Nan() const
{
	return nan;
}

bool Fractional::operator==(const Fractional& other) const
{
	if (negative != other.negative || infinity != other.infinity || afterDot != other.afterDot || number.size() != other.number.size() || (nan || other.nan))
		return false;
	if (infinity && other.infinity)
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
	if (nan || other.nan)
		return (negative == other.negative) ? (false) : (negative);
	if (negative != other.negative)
		return negative;
	if (infinity || other.infinity)
	{
		if (infinity && other.infinity)
			return false;
		return negative ? (infinity) : (!infinity);
	}
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
	if (nan || other.nan)
		return (negative == other.negative) ? (false) : (other.negative);
	if (negative != other.negative)
		return !negative;
	if (infinity || other.infinity)
	{
		if (infinity && other.infinity)
			return false;
		return negative ? (other.infinity) : (!other.infinity);
	}
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

#pragma region math operators

Fractional Fractional::operator-() const
{
	auto res = *this;
	return res.MadeOpposite();
}

Fractional Fractional::operator+(const Fractional& other) const
{
	Fractional res = *this;
	return res += other;
}
Fractional Fractional::operator-(const Fractional& other) const
{
	Fractional res = *this;
	return res -= other;
}
Fractional Fractional::operator*(const Fractional& other) const
{
	if (nan || other.nan)
		return FabrickNan(negative != other.negative);
	if (infinity || other.infinity)
		return (Null() || other.Null()) ? (FabrickNan(negative != other.negative)) : (FabrickInfinity(negative != other.negative));
	if (Null() || other.Null())
		return Fractional{};

	Fractional res;
	res.maxAfterDot = maxAfterDot;
	multiplication(number, other.number, res.number);
	res.afterDot = afterDot + other.afterDot;
	res.RemoveLastZeros();
	res.negative = negative != other.negative;
	return res;
}
Fractional Fractional::operator/(const Fractional& other) const
{
	Fractional res = *this;
	return res /= other;
}

Fractional& Fractional::operator+=(const Fractional& other)
{
	if (nan || other.nan)
		return this->operator=(FabrickNan(negative));
	if (infinity || other.infinity)
	{
		if (infinity && other.infinity)
			return (negative != other.negative) ? this->operator=(Fractional{}) : this->operator=(FabrickInfinity(negative));
		return this->operator=(FabrickInfinity(infinity ? negative : other.negative));
	}
	if (negative && !other.negative)
		return (this->operator=(other.operator-(MadeOpposite())));
	if (other.negative)
		return this->operator-=(other.operator-());
	if (afterDot < other.afterDot)
	{
		number.push_begin_count(other.afterDot - afterDot);
		afterDot += other.afterDot - afterDot;
	}
	size_t plus = (afterDot > other.afterDot) ? (afterDot - other.afterDot) : (0uLL);
	number.resize(std::max(number.size(), other.number.size()) + 1);
	size_t size = std::max(number.size(), other.number.size());
	for (size_t i = 0; i < size; ++i)
	{
		number[i + plus] += other.number[i];
		if (number[i + plus] > 9)
		{
			++number[i + 1 + plus];
			number[i + plus] -= 10;
		}
	}
	RemoveLastZeros();
	return *this;
}
Fractional& Fractional::operator-=(const Fractional& other)
{
	if (nan || other.nan)
		return this->operator=(FabrickNan(negative));
	if (infinity || other.infinity)
	{
		if (infinity && other.infinity)
			return (negative == other.negative) ? (this->operator=(Fractional{})) : (this->operator=(FabrickInfinity(negative)));
		return this->operator=(FabrickInfinity(infinity ? negative : other.negative));
	}
	if (other.negative)
		return this->operator+=(other.operator-());
	if (negative)
		return MadeOpposite().operator+=(other).MadeOpposite();
	if (this->operator<(other))
		return this->operator=(other.operator-(*this)).MadeOpposite();
	if (afterDot < other.afterDot)
	{
		number.push_begin_count(other.afterDot - afterDot);
		afterDot += other.afterDot - afterDot;
	}
	size_t plus = (afterDot > other.afterDot) ? (afterDot - other.afterDot) : (0uLL);
	number.resize(std::max(number.size(), other.number.size()) + 1);
	size_t size = std::max(number.size(), other.number.size());
	for (size_t i = 0; i < size; ++i)
	{
		number[i + plus] -= other.number[i];
		if (number[i + plus] < 0)
		{
			--number[i + 1 + plus];
			number[i + plus] += 10;
		}
	}
	RemoveLastZeros();
	return *this;
}
Fractional& Fractional::operator*=(const Fractional& other)
{
	return this->operator=(this->operator*(other));
}
Fractional& Fractional::operator/=(const Fractional& other)
{
	if (nan || other.nan)
		return this->operator=(FabrickNan(negative != other.negative));
	if (other.Null())
		return (Null() || infinity) ? this->operator=(FabrickNan(negative != other.negative)) : this->operator=(FabrickInfinity(negative != other.negative));
	if (infinity || other.infinity)
	{
		if (infinity && other.infinity)
		{
			Integer res{ 1 };
			res.negative = negative != other.negative;
			return this->operator=(res);
		}
		if (infinity)
		{
			negative = negative != other.negative;
			return *this;
		}
		Assign(0uLL);
		return *this;
	}
	if (Null())
	{
		negative = false;
		return *this;
	}
	Fractional right = other;
	size_t pr = std::max(afterDot, other.afterDot);
	Multi10pow(maxAfterDot + pr);
	right.Multi10pow(pr);
	DecVector res;
	division::division(number, right.number, res);
	number = res;
	negative = negative != other.negative;
	afterDot = maxAfterDot;
	RemoveLastZeros();
	while (afterDot >= number.size())
		number.push_back('\000');
	return *this;
}

#pragma endregion

size_t Fractional::AfterDot() const
{
	return afterDot;
}
size_t Fractional::MaxSignsAfterDotDiving() const
{
	return maxAfterDot;
}
Fractional& Fractional::SetMaxSignsAfterDotDiving(size_t val)
{
	maxAfterDot = val;
	return *this;
}

Fractional& Fractional::MadeOpposite()
{
	negative = Null() ? false : !negative;
	return *this;
}

void Fractional::RemoveLastZeros()
{
	size_t i = 0;
	while (number[i] == '\000' && afterDot != 0 && i < number.size())
		i++, afterDot--;
	number.erase(0, i);
	while (number.back() == '\000' && (number.size() - afterDot) > 1)
		number.pop_back();
}

Fractional Fractional::FabrickNan(bool negative)
{
	Fractional res;
	res.negative = negative;
	res.nan = true;
	return res;
}
Fractional Fractional::FabrickInfinity(bool negative)
{
	Fractional res;
	res.negative = negative;
	res.infinity = true;
	return res;
}

void Fractional::Multi10pow(size_t count)
{
	if (afterDot < count)
	{
		count -= afterDot;
		afterDot = 0;
		number.push_begin_count(count);
	}
	else
	{
		afterDot -= count;
	}
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

#pragma region class Math

Integer Math::min(const Integer& a, const Integer& b)
{
	return (a < b) ? a : b;
}
Integer Math::max(const Integer& a, const Integer& b)
{
	return (a > b) ? a : b;
}

Fractional Math::min(const Fractional& a, const Fractional& b)
{
	return (a < b) ? a : b;
}
Fractional Math::max(const Fractional& a, const Fractional& b)
{
	return (a > b) ? a : b;
}

Integer Math::abs(const Integer& val)
{
	return val.Negative() ? -val : val;
}
Fractional Math::abs(const Fractional& val)
{
	return val.Negative() ? -val : val;
}

Fractional Math::round(const Fractional& x, size_t afterDot)
{
	if (x.afterDot <= afterDot || x.nan || x.infinity)
		return x;
	Fractional res;
	res.number.clear();
	res.number.reserve(x.number.size() - (x.afterDot - afterDot));
	for (size_t i = (x.afterDot - afterDot); i < x.number.size(); ++i)
		res.number.push_back(x.number[i]);
	res.negative = x.negative;
	res.afterDot = afterDot;
	if (x.number[(x.afterDot - afterDot - 1)] > 4)
	{
		++res.number[0];
		for (size_t i = 0; i < res.number.size(); ++i)
		{
			if (res.number[i] > 9)
			{
				res.number[i] -= 10;
				if (res.number.size() - 1 == i)
					res.number.push_back('\000');
				++res.number[i + 1];
			}
			else
				break;
		}
	}
	res.RemoveLastZeros();
	return res;
}
Fractional Math::round(const Fractional& x)
{
	return round(x, 0);
}
Fractional Math::floor(const Fractional& x)
{
	if (x.afterDot == 0 || x.nan || x.infinity)
		return x;
	Fractional res;
	res.number.clear();
	res.number.reserve(x.number.size() - x.afterDot);
	for (size_t i = x.afterDot; i < x.number.size(); ++i)
		res.number.push_back(x.number[i]);
	res.negative = x.negative;
	if (res.negative)
		res -= 1;
	return res;
}

Fractional Math::modf(const Fractional a, Fractional* intpart)
{
	if (a.infinity || a.nan)
		return a;
	if (intpart != nullptr)
	{
		intpart->infinity = intpart->nan = false;
		intpart->maxAfterDot = a.maxAfterDot;
		intpart->afterDot = 0;
		intpart->number.clear();
		for (size_t i = a.afterDot; i < a.number.size(); ++i)
			intpart->number.push_back(a.number[i]);
		intpart->negative = a.negative;
	}
	Fractional res;
	if (a.afterDot != 0)
	{
		res.number.clear();
		for (size_t i = 0; i < a.afterDot; ++i)
			res.number.push_back(a.number[i]);
		res.number.push_back('\000');
		res.afterDot = a.afterDot;
		res.negative = a.negative;
	}
	return res;
}

Integer Math::pow(const Integer& a, const Integer& b)
{
	if (a.Null() && b.Null())
		return Integer::FabrickNan(false);
	if (a.Null())
		return a;
	if (b.Negative())
		return 0_b;
	Integer res = 1;
	for (Integer i = 0_b; i < b; i++)
		res *= a;
	return res;
}
Fractional Math::pow(const Fractional& a, const Integer& b)
{
	if (a.Null() && b.Null())
		return Integer::FabrickNan(false);
	if (a.Null())
		return a;
	if (b.Negative())
	{
		Fractional pr;
		pr.SetMaxSignsAfterDotDiving(a.MaxSignsAfterDotDiving());
		return pr / pow(a, -b);
	}
	Fractional res = 1;
	for (Integer i = 0_b; i < b; i++)
		res *= a;
	return res;
}

Fractional Math::exp(const Fractional& x, const Fractional& epsilon)
{
	if (x.nan || epsilon.nan)
		return Fractional::FabrickNan(false);
	if (x.infinity)
		return x;
	Fractional res = x + 1;
	Fractional prX = x;
	Fractional prF = 1;
	Fractional term;
	prF.maxAfterDot = term.maxAfterDot = x.maxAfterDot;
	Fractional i = 2;
	while (true)
	{
		prX *= x;
		prF *= i;
		term = prX / prF;
		if (abs(term) < epsilon)
			break;
		res += term;
		i += 1;
	}
	return res;
}
Fractional Math::exp(const Fractional& x)
{
	return exp(x, "0.00001"_fb);
}
Fractional Math::exp(const Integer& x)
{
	return exp(static_cast<Fractional>(x));
}

Fractional Math::ln(const Fractional& z, const Fractional& epsilon)
{
	if (z.negative || z.nan || epsilon.nan || z.Null())
	{
		Fractional res;
		res.maxAfterDot = z.maxAfterDot;
		res.nan = true;
		res.negative = z.negative;
		return res;
	}
	if (z.infinity)
		return z;
	Fractional x = (z - 1) / (z + 1);
	Fractional res = x;
	Fractional prX = x;
	Fractional i = 1;
	Fractional term;
	term.maxAfterDot = z.maxAfterDot;
	x *= x;
	while (true)
	{
		prX *= x;
		i += 2;
		term = prX / i;
		if (term < epsilon)
			break;
		res += term;
	}
	return res * 2;
}
Fractional Math::ln(const Fractional& x)
{
	return ln(x, "0.00001"_fb);
}
Fractional Math::ln(const Integer& x)
{
	return ln(static_cast<Fractional>(x));
}

Integer Math::fact(const Integer& a)
{
	if (a.Null())
		return 0_b;
	if (a.Negative())
		return Integer::FabrickNan(true);
	Integer res = 1;
	for (Integer i = 1_b; i <= a; i++)
		res *= i;
	return res;
}

bool Math::prime(const Integer& num)
{
	if (num.Negative() || num.Null())
		return false;
	if (num == 2_b)
		return true;
	auto pr = static_cast<short>(num % 10);
	if (pr % 2 == 0 || pr % 5 == 0 || num == 1_b)
		return false;
	for (Integer i = 3; i < num; i += 2_b)
		if ((num % i).Null())
			return false;
	return true;
}

#pragma endregion