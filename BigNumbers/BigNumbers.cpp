#include "BigNumbers.hpp"

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
	static bool null(const DecVector& v)
	{
		for (size_t i = 0; i < v.size(); ++i)
			if (v[i] != '\000')
				return false;
		return true;
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
		if (null(pr))
		{
			pr_mul.clear();
			pr_mul.push_back('\000');
			prC = '\000';
			return;
		}
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
	number(other.number), negative(other.negative), type(other.type) {}
Integer::Integer(Integer&& other) noexcept :
	number(other.number), negative(other.negative), type(other.type) {}

void Integer::Assign(long long num)
{
	negative = (num < 0);
	type = NumSpecialType::None;
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
	type = NumSpecialType::None;
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
	type = NumSpecialType::None;
	negative = (num[0] == '-') ? (num++, true) : (false);
	number.clear();
	if (num[0] == 'i' && num[1] == 'n' && num[2] == 'f')
		return (type = NumSpecialType::Infinity), void();
	if (num[0] == 'n' && num[1] == 'a' && num[2] == 'n')
		return (type = NumSpecialType::Nan), void();
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
	type = NumSpecialType::Nan;
	negative = (num[0] == '-') ? (num++, true) : (false);
	number.clear();
	if (num[0] == L'i' && num[1] == L'n' && num[2] == L'f')
		return (type = NumSpecialType::Infinity), void();
	if (num[0] == L'n' && num[1] == L'a' && num[2] == L'n')
		return (type = NumSpecialType::Nan), void();
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
		type = other.type;
	}
	return *this;
}
Integer& Integer::operator=(Integer&& other) noexcept
{
	if (&other != this)
	{
		number = other.number;
		negative = other.negative;
		type = other.type;
	}
	return *this;
}

#pragma endregion

#pragma region bool operators

bool Integer::Null() const
{
	return ((number.empty()) || (number.size() == 1 && number[0] == '\000')) && type == NumSpecialType::None;
}
bool Integer::Negative() const
{
	return negative;
}
bool Integer::Infinity() const
{
	return type == NumSpecialType::Infinity;
}
bool Integer::Nan() const
{
	return type == NumSpecialType::Nan;
}

bool Integer::operator==(const Integer& other) const
{
	if ((negative != other.negative) || (type != other.type) || number.size() != other.number.size())
		return false;
	if (type == NumSpecialType::Infinity && other.type == NumSpecialType::Infinity)
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
	if (type == NumSpecialType::Nan || other.type == NumSpecialType::Nan)
		return (negative == other.negative) ? (false) : (negative);
	if (negative != other.negative)
		return negative;
	if (type == NumSpecialType::Infinity || other.type == NumSpecialType::Infinity)
	{
		if (type == NumSpecialType::Infinity && other.type == NumSpecialType::Infinity)
			return false;
		return negative ? (type == NumSpecialType::Infinity) : (type != NumSpecialType::Infinity);
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
	if (type == NumSpecialType::Nan || other.type == NumSpecialType::Nan)
		return (negative == other.negative) ? (false) : (other.negative);
	if (negative != other.negative)
		return other.negative;
	if (type == NumSpecialType::Infinity || other.type == NumSpecialType::Infinity)
	{
		if (type == NumSpecialType::Infinity && other.type == NumSpecialType::Infinity)
			return false;
		return negative ? (other.type == NumSpecialType::Infinity) : (other.type != NumSpecialType::Infinity);
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
	if (type == NumSpecialType::Nan)
		return res;
	if (type == NumSpecialType::Infinity)
		return negative ? std::numeric_limits<long long>::min() : std::numeric_limits<long long>::max();
	const size_t maxSize = static_cast<size_t>(std::log10(std::numeric_limits<long long>::max())) + 1;
	for (size_t i = 0; i < std::min(number.size(), maxSize); i++)
		res += ((negative) ? (-number[i]) : (number[i])) * static_cast<size_t>(std::pow(10, i));
	return res;
}
unsigned long long	Integer::ToULLong() const
{
	unsigned long long res = 0;
	if (type == NumSpecialType::Nan)
		return res;
	if (type == NumSpecialType::Infinity)
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
	if (type == NumSpecialType::Nan)
		return res + "nan";
	if (type == NumSpecialType::Infinity)
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
	if (type == NumSpecialType::Nan || other.type == NumSpecialType::Nan)
		return FabrickNan(negative != other.negative);
	if (type == NumSpecialType::Infinity || other.type == NumSpecialType::Infinity)
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
	if (type == NumSpecialType::Nan || other.type == NumSpecialType::Nan)
		return this->operator=(FabrickNan(negative));
	if (type == NumSpecialType::Infinity || other.type == NumSpecialType::Infinity)
	{
		if (type == NumSpecialType::Infinity && other.type == NumSpecialType::Infinity)
			return (negative != other.negative) ? this->operator=(Integer{}) : this->operator=(FabrickInfinity(negative));
		return this->operator=(FabrickInfinity(type == NumSpecialType::Infinity ? negative : other.negative));
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
	if (type == NumSpecialType::Nan || other.type == NumSpecialType::Nan)
		return this->operator=(FabrickNan(negative));
	if (type == NumSpecialType::Infinity || other.type == NumSpecialType::Infinity)
	{
		if (type == NumSpecialType::Infinity && other.type == NumSpecialType::Infinity)
			return (negative == other.negative) ? (this->operator=(Integer{})) : (this->operator=(FabrickInfinity(negative)));
		return this->operator=(FabrickInfinity(type == NumSpecialType::Infinity ? negative : other.negative));
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
	if (type == NumSpecialType::Nan || other.type == NumSpecialType::Nan)
		return this->operator=(FabrickNan(negative != other.negative));
	if (other.Null())
		return (Null() || type == NumSpecialType::Infinity) ? this->operator=(FabrickNan(negative != other.negative)) : this->operator=(FabrickInfinity(negative != other.negative));
	if (type == NumSpecialType::Infinity || other.type == NumSpecialType::Infinity)
	{
		if (type == NumSpecialType::Infinity && other.type == NumSpecialType::Infinity)
		{
			Integer res{ 1 };
			res.negative = negative != other.negative;
			return this->operator=(res);
		}
		if (type == NumSpecialType::Infinity)
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
	if (type == NumSpecialType::Infinity || other.type == NumSpecialType::Infinity || type == NumSpecialType::Nan || other.type == NumSpecialType::Nan || other.Null())
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
	num.type = NumSpecialType::None;
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
	res.type = NumSpecialType::Infinity;
	res.negative = negative;
	return res;
}
Integer Integer::FabrickNan(bool negative)
{
	Integer res;
	res.type = NumSpecialType::Nan;
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
	number{ val.number }, afterDot{ val.afterDot }, negative{ val.negative }, type{val.type}, maxAfterDot{ val.maxAfterDot } {}
Fractional::Fractional(Fractional&& val) noexcept :
	number{ val.number }, afterDot{ val.afterDot }, negative{ val.negative }, type{val.type}, maxAfterDot{ val.maxAfterDot } {}
Fractional::Fractional(const Integer& val) :
	number{ val.number }, negative{ val.negative }, type{val.type}, afterDot{ 0 } {}

void Fractional::Assign(const char* num)
{
	if (num == nullptr)
		return Assign(0uLL);
	type = NumSpecialType::None;
	negative = (num[0] == '-') ? (num++, true) : (false);
	number.clear();
	if (num[0] == 'i' && num[1] == 'n' && num[2] == 'f')
		return (type = NumSpecialType::Infinity), void();
	if (num[0] == 'n' && num[1] == 'a' && num[2] == 'n')
		return (type = NumSpecialType::Nan), void();
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
	maxAfterDot = std::max(afterDot, maxAfterDot);
	RemoveLastZeros();
}
void Fractional::Assign(const wchar_t* num)
{
	if (num == nullptr)
		return Assign(0uLL);
	type = NumSpecialType::None;
	negative = (num[0] == L'-') ? (num++, true) : (false);
	number.clear();
	if (num[0] == L'i' && num[1] == L'n' && num[2] == L'f')
		return (type = NumSpecialType::Infinity), void();
	if (num[0] == L'n' && num[1] == L'a' && num[2] == L'n')
		return (type = NumSpecialType::Nan), void();
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
	maxAfterDot = std::max(afterDot, maxAfterDot);
	number.reverse();
	RemoveLastZeros();
}
void Fractional::Assign(long long num)
{
	negative = (num < 0LL);
	type = NumSpecialType::None;
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
	negative = false;
	type = NumSpecialType::None;
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
	afterDot = 0;
	type = NumSpecialType::None;
	number.clear();
	if (isnan(val))
	{
		type = NumSpecialType::Nan;
		number.push_back('\000');
		return;
	}
	if (isinf(val))
	{
		type = NumSpecialType::Infinity;
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
	maxAfterDot = std::max(afterDot, maxAfterDot);
}

Fractional& Fractional::operator=(const Fractional& other)
{
	if (this == &other)
		return *this;
	negative = other.negative;
	type = other.type;
	afterDot = other.afterDot;
	maxAfterDot = std::max(other.maxAfterDot, maxAfterDot);
	number = (type != NumSpecialType::None) ? DecVector{} : other.number;
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
	return static_cast<char>(ToLLong());
}
Fractional::operator char() const
{
	return static_cast<char>(ToLLong());
}
Fractional::operator int() const
{
	return static_cast<int>(ToLLong());
}
Fractional::operator long long() const
{
	return ToLLong();
}
Fractional::operator unsigned char() const
{
	return static_cast<unsigned char>(ToLLong());
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
	if (type == NumSpecialType::Infinity)
		return negative ? "-inf" : "inf";
	if (type == NumSpecialType::Nan)
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
	if (type == NumSpecialType::Infinity)
		return negative ? std::numeric_limits<long long>::min() : std::numeric_limits<long long>::max();
	if (type == NumSpecialType::Nan)
		return long long{};
	long long res = 0;
	const size_t maxSize = static_cast<size_t>(std::log10(std::numeric_limits<long long>::max())) + 1;
	for (size_t i = 0; i < std::min(number.size() - afterDot, maxSize); i++)
		res += negative ? -number[i + afterDot] : number[i + afterDot] * static_cast<long long>(std::pow(10, i));
	return res;
}
unsigned long long	Fractional::ToULLong()	const
{
	if (type == NumSpecialType::Infinity)
		return negative ? std::numeric_limits<unsigned long long>::min() : std::numeric_limits<unsigned long long>::max();
	if (type == NumSpecialType::Nan)
		return unsigned long long{};
	unsigned long long res = 0;
	const size_t maxSize = static_cast<size_t>(std::log10(std::numeric_limits<long long>::max()));
	for (size_t i = 0; i < std::min(number.size() - afterDot, maxSize); i++)
		res += number[i + afterDot] * static_cast<size_t>(std::pow(10, i));
	return res;
}
long double			Fractional::ToLDouble() const
{
	if (type == NumSpecialType::Infinity)
		return negative ? (-1.l / sin(0)) : (1.l / sin(0));
	if (type == NumSpecialType::Nan)
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
	if (afterDot == 0)
		res.number = number;
	else
	{
		res.number.clear();
		res.number.reserve(number.size() - afterDot);
		for (size_t i = afterDot; i < number.size(); i++)
			res.number.push_back(number[i]);
	}
	res.negative = negative;
	res.type = type;
	return res;
}

#pragma endregion

#pragma region bool operators

bool Fractional::Null() const
{
	return ((number.empty()) || (number.size() == 1 && number[0] == '\000')) && !(type != NumSpecialType::None || afterDot != 0);
}
bool Fractional::Negative() const
{
	return negative;
}
bool Fractional::Infinity() const
{
	return type == NumSpecialType::Infinity;
}
bool Fractional::Nan() const
{
	return type == NumSpecialType::Nan;
}

bool Fractional::operator==(const Fractional& other) const
{
	if (negative != other.negative || type == NumSpecialType::Infinity || other.type == NumSpecialType::Infinity || afterDot != other.afterDot || number.size() != other.number.size() || (type == NumSpecialType::Nan || other.type == NumSpecialType::Nan))
		return false;
	if (type == NumSpecialType::Infinity && other.type == NumSpecialType::Infinity)
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
	if (type == NumSpecialType::Nan || other.type == NumSpecialType::Nan)
		return (negative == other.negative) ? (false) : (negative);
	if (negative != other.negative)
		return negative;
	if (type == NumSpecialType::Infinity || other.type == NumSpecialType::Infinity)
	{
		if (type == NumSpecialType::Infinity && other.type == NumSpecialType::Infinity)
			return false;
		return negative ? (type == NumSpecialType::Infinity) : (type != NumSpecialType::Infinity);
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
	if (type == NumSpecialType::Nan || other.type == NumSpecialType::Nan)
		return (negative == other.negative) ? (false) : (other.negative);
	if (negative != other.negative)
		return !negative;
	if (type == NumSpecialType::Infinity || other.type == NumSpecialType::Infinity)
	{
		if (type == NumSpecialType::Infinity && other.type == NumSpecialType::Infinity)
			return false;
		return negative ? (other.type == NumSpecialType::Infinity) : (other.type != NumSpecialType::Infinity);
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
	if (type == NumSpecialType::Nan || other.type == NumSpecialType::Nan)
		return FabrickNan(negative != other.negative);
	if (type == NumSpecialType::Infinity || other.type == NumSpecialType::Infinity)
		return (Null() || other.Null()) ? (FabrickNan(negative != other.negative)) : (FabrickInfinity(negative != other.negative));
	if (Null() || other.Null())
		return Fractional{};

	Fractional res;
	res.maxAfterDot = std::max(maxAfterDot, other.maxAfterDot);
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
Fractional Fractional::operator%(const Fractional& other) const
{
	Fractional res = *this;
	return res %= other;
}

Fractional& Fractional::operator+=(const Fractional& other)
{
	if (type == NumSpecialType::Nan || other.type == NumSpecialType::Nan)
		return this->operator=(FabrickNan(negative));
	if (type == NumSpecialType::Infinity || other.type == NumSpecialType::Infinity)
	{
		if (type == NumSpecialType::Infinity && other.type == NumSpecialType::Infinity)
			return (negative != other.negative) ? this->operator=(Fractional{}) : this->operator=(FabrickInfinity(negative));
		return this->operator=(FabrickInfinity(type == NumSpecialType::Infinity ? negative : other.negative));
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
	maxAfterDot = std::max(maxAfterDot, other.maxAfterDot);
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
	if (type == NumSpecialType::Nan || other.type == NumSpecialType::Nan)
		return this->operator=(FabrickNan(negative));
	if (type == NumSpecialType::Infinity || other.type == NumSpecialType::Infinity)
	{
		if (type == NumSpecialType::Infinity && other.type == NumSpecialType::Infinity)
			return (negative == other.negative) ? (this->operator=(Fractional{})) : (this->operator=(FabrickInfinity(negative)));
		return this->operator=(FabrickInfinity(type == NumSpecialType::Infinity ? negative : other.negative));
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
	maxAfterDot = std::max(maxAfterDot, other.maxAfterDot);
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
	if (type == NumSpecialType::Nan || other.type == NumSpecialType::Nan)
		return this->operator=(FabrickNan(negative != other.negative));
	if (other.Null())
		return (Null() || type == NumSpecialType::Infinity) ? this->operator=(FabrickNan(negative != other.negative)) : this->operator=(FabrickInfinity(negative != other.negative));
	if (type == NumSpecialType::Infinity || other.type == NumSpecialType::Infinity)
	{
		if (type == NumSpecialType::Infinity && other.type == NumSpecialType::Infinity)
		{
			Integer res{ 1 };
			res.negative = negative != other.negative;
			return this->operator=(res);
		}
		if (type == NumSpecialType::Infinity)
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
	maxAfterDot = std::max(maxAfterDot, other.maxAfterDot);
	Fractional right = other;
	size_t pr = std::max(afterDot, other.afterDot);
	Multi10pow(maxAfterDot + pr);
	right.Multi10pow(pr);
	DecVector res;
	DecVector ostat;
	division::division(number, right.number, res, ostat);
	number = res;
	negative = negative != other.negative;
	afterDot = maxAfterDot;
	RemoveLastZeros();
	while (afterDot >= number.size())
		number.push_back('\000');
	return *this;
}
Fractional& Fractional::operator%=(const Fractional& other)
{
	if (type == NumSpecialType::Infinity || other.type == NumSpecialType::Infinity || type == NumSpecialType::Nan || other.type == NumSpecialType::Nan || other.Null())
		return this->operator=(FabrickNan(false));
	if (Null())
	{
		negative = false;
		return *this;
	}
	maxAfterDot = std::max(maxAfterDot, other.maxAfterDot);
	negative = false;
	Fractional right = other;
	Fractional left = *this;
	size_t pr = std::max(afterDot, other.afterDot);
	Multi10pow(maxAfterDot + pr);
	right.Multi10pow(pr);
	DecVector res;
	division::division(number, right.number, res);

	res.erase(0, maxAfterDot);
	

	multiplication(res, other.number, number);
	afterDot = other.afterDot;
	RemoveLastZeros();

	return operator=(left -= *this);
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
void Fractional::EraseAfterMaxAfterDot()
{
	if (afterDot <= maxAfterDot)
		return;
	number.erase(0, afterDot - maxAfterDot);
	afterDot = maxAfterDot;
	RemoveLastZeros();
}

Fractional Fractional::FabrickNan(bool negative)
{
	Fractional res;
	res.negative = negative;
	res.type = NumSpecialType::Nan;
	return res;
}
Fractional Fractional::FabrickInfinity(bool negative)
{
	Fractional res;
	res.negative = negative;
	res.type = NumSpecialType::Infinity;
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
		afterDot -= count;
	while (number.back() == '\000' && number.size() != 1)
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
Fractional operator""_fb(const long double val)
{
	return Fractional{ val };
}

#pragma endregion