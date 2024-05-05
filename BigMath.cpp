#include "BigMath.hpp"

#pragma region class Math

Fractional Math::defaultEpsilon = "0.0000000001";

Fractional	Math::GetEpsolon()
{
	return defaultEpsilon;
}
void		Math::SetEpsilon(const Fractional& val)
{
	if (!(val.Null() || val.type != NumSpecialType::None || val.negative))
		defaultEpsilon = val;
}

#pragma region std

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

Integer		Math::abs(const Integer& val)
{
	return val.Negative() ? -val : val;
}
Fractional	Math::abs(const Fractional& val)
{
	return val.Negative() ? -val : val;
}

Fractional Math::round(const Fractional& x, size_t afterDot)
{
	if (x.afterDot <= afterDot || x.type != NumSpecialType::None)
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
	if (x.afterDot == 0 || x.type != NumSpecialType::None)
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
	if (a.type != NumSpecialType::None)
		return a;
	if (intpart != nullptr)
	{
		intpart->type = NumSpecialType::None;
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

#pragma endregion

#pragma region nroot

Fractional Math::nroot(const Fractional& a, const Integer& n, const Fractional& epsilon)
{
	Fractional res = a, pr;
	Integer inmin = n - 1;
	Fractional fnmin = inmin;
	Fractional fn = n;
	while (true)
	{
		pr = (res * fnmin + a / pow(res, inmin)) / fn;
		if (abs(res - pr) < epsilon)
		{
			res = pr;
			break;
		}
		res = pr;
	}
	return res;
}
Fractional Math::nroot(const Fractional& a, const Integer& n)
{
	return nroot(a, n, defaultEpsilon);
}
Fractional Math::nroot(const Integer& a, const Integer& n)
{
	return nroot(static_cast<Fractional>(a), n, defaultEpsilon);
}

Fractional Math::nroot(const Fractional& a, const Fractional& n, const Fractional& epsilon)
{
	return pow(a, Fractional(1) / n, epsilon);
}
Fractional Math::nroot(const Fractional& a, const Fractional& n)
{
	return nroot(a, n, defaultEpsilon);
}
Fractional Math::nroot(const Integer& a, const Fractional& n)
{
	return nroot(a, n, defaultEpsilon);
}

#pragma endregion

#pragma region pow

Fractional Math::pow(const Integer& a, const Integer& b)
{
	if (a.type == NumSpecialType::Nan)
		return Fractional::FabrickNan(false);
	if (a.type == NumSpecialType::Infinity)
		return b.Null() ? Fractional::FabrickNan(a.negative) : Fractional::FabrickInfinity(a.negative ? !even(b) : false);
	if (b.type == NumSpecialType::Infinity)
		return a.Null() || a == 1 ? a : b;
	if (a.Null())
		return b.Null() ? Fractional::FabrickNan(false) : a;
	Integer res = 1;
	Integer nx = a;

	Integer exp = b.Negative() ? b : -b;

	while (!exp.Null())
	{
		if (!Math::even(exp))
			res *= nx;
		nx *= nx;
		exp /= 2;
	}
	return !b.Negative() ? res : Fractional(1) / res;
}
Fractional Math::pow(const Fractional& a, const Integer& b)
{
	if (a.type == NumSpecialType::Nan || b.type == NumSpecialType::Nan)
		return Fractional::FabrickNan(false);
	if (a.type == NumSpecialType::Infinity)
		return b.Null() ? Fractional::FabrickNan(a.negative) : Fractional::FabrickInfinity(a.negative ? !even(b) : false);
	if (b.type == NumSpecialType::Infinity)
		return a.Null() || a == 1 ? a : b;
	if (a.Null())
		return b.Null() ? Fractional::FabrickNan(false) : a;
	if (b.Null())
		return 1;

	Fractional res = 1;
	res.SetMaxSignsAfterDotDiving(a.MaxSignsAfterDotDiving());
	Fractional nx = a;

	Integer exp = b.Negative() ? b : -b;

	while (!exp.Null())
	{
		if (!Math::even(exp))
			res *= nx;
		nx *= nx;
		exp /= 2;
	}
	return !b.Negative() ? res : Fractional(1) / res;
}
Fractional Math::pow(const Integer& a, const Fractional& b)
{
	return pow(static_cast<Fractional>(a), b);
}
Fractional Math::pow(const Fractional& a, const Fractional& b)
{
	return pow(a, b, defaultEpsilon);
}
Fractional Math::pow(const Fractional& a, const Fractional& b, const Fractional& epsilon)
{
	if (b.afterDot == 0)
		return pow(a, static_cast<Integer>(b));
	if (a.type == NumSpecialType::Nan || b.type == NumSpecialType::Nan)
		return Fractional::FabrickNan(a.negative != b.negative);
	if (a.type == NumSpecialType::Infinity)
		return b.Null() ? Fractional::FabrickNan(a.negative) : Fractional::FabrickInfinity(false);
	if (b.type == NumSpecialType::Infinity)
		return a.Null() || a == 1 ? a : b;
	if (a.Null())
		return b.Null() ? Fractional::FabrickNan(false) : a;

	Integer ie = static_cast<Integer>(b);
	if (ie.Null())
		return exp(b * ln(a, epsilon), epsilon);
	Fractional res = pow(a, ie);
	Fractional pr = b - static_cast<Fractional>(ie);
	return res * exp(pr * ln(a, epsilon), epsilon);
}

#pragma endregion

#pragma region sqrt

Fractional Math::sqrt(const Fractional& x, const Fractional& epsilon)
{
	if (x.type == NumSpecialType::Nan || epsilon.type == NumSpecialType::Nan || x.negative)
		return Fractional::FabrickNan(false);
	if (x.type == NumSpecialType::Infinity)
		return x;
	Fractional res = x / 2;
	Fractional prev_x;
	prev_x.maxAfterDot = res.maxAfterDot = std::max(x.maxAfterDot, epsilon.maxAfterDot);
	do {
		prev_x = res;
		res = (res + x / res) / 2;
	} while (Math::abs(res - prev_x) >= epsilon);

	return res;
}
Fractional Math::sqrt(const Fractional& x)
{
	return sqrt(x, defaultEpsilon);
}
Fractional Math::sqrt(const Integer& x)
{
	return sqrt(static_cast<Fractional>(x), defaultEpsilon);
}

#pragma endregion

#pragma region exp

Fractional Math::exp(const Fractional& x, const Fractional& epsilon)
{
	if (x.type == NumSpecialType::Nan || epsilon.type == NumSpecialType::Nan)
		return Fractional::FabrickNan(false);
	if (x.type == NumSpecialType::Infinity)
		return x;
	Fractional tx = x.negative ? -x : x;
	Fractional res = tx + 1;
	Fractional prX = tx;
	Fractional prF = 1;
	Fractional term;
	prF.maxAfterDot = term.maxAfterDot = std::max(tx.maxAfterDot, epsilon.maxAfterDot);
	Fractional i = 2;
	do {
		prX *= tx;
		prF *= i;
		term = prX / prF;
		res += term;
		i += 1;
	} while (abs(term) >= epsilon);
	return x.negative ? Fractional(1) / round(res, epsilon.afterDot) : round(res, epsilon.afterDot);
}
Fractional Math::exp(const Fractional& x)
{
	return exp(x, defaultEpsilon);
}
Fractional Math::exp(const Integer& x)
{
	return exp(static_cast<Fractional>(x));
}

Fractional Math::expFast(const Fractional& x, const Fractional& epsilon)
{
	Integer ai = static_cast<Integer>(x);
	Fractional af = x - static_cast<Fractional>(ai);

	return Math::pow(Math::exp(1, epsilon), ai) * Math::exp(af, epsilon);
}
Fractional Math::expFast(const Integer& x)
{
	return expFast(static_cast<Fractional>(x), defaultEpsilon);
}
Fractional Math::expFast(const Fractional& x)
{
	return expFast(x, defaultEpsilon);
}

#pragma endregion

#pragma region ln

Fractional Math::ln(const Fractional& z, const Fractional& epsilon)
{
	if (z.negative || z.type == NumSpecialType::Nan || epsilon.type == NumSpecialType::Nan || z.Null())
		return Fractional::FabrickNan(z.negative);
	if (z.type == NumSpecialType::Infinity)
		return z;
	Fractional x = (z - 1) / (z + 1);
	Fractional res = x;
	Fractional prX = x;
	Fractional i = 1;
	Fractional term;
	term.maxAfterDot = std::max(z.maxAfterDot, epsilon.maxAfterDot);
	x *= x;
	do {
		prX *= x;
		i += 2;
		term = prX / i;
		res += term;
	} while (term >= epsilon);
	return round(res * 2, epsilon.afterDot);
}
Fractional Math::ln(const Fractional& x)
{
	return ln(x, defaultEpsilon);
}
Fractional Math::ln(const Integer& x)
{
	return ln(static_cast<Fractional>(x));
}

#pragma endregion

#pragma region special

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
	if (num.negative || num.Null() || num.type != NumSpecialType::None)
		return false;
	if (num.number.size() == 1 && num.number[0] == '\002')
		return true;
	auto pr = static_cast<short>(num.number[0]);
	if (pr % 2 == 0 || pr % 5 == 0 || pr == 0 || (num.number.size() == 1 && num.number[0] == '\001'))
		return false;
	auto sqr = sqrt(num);
	std::cout << sqr << '\n';
	if (sqr.afterDot == 0)
		return false;
	for (Integer i = 3, end = static_cast<Integer>(sqr); i <= end; i += 2_b)
		if ((num % i).Null())
			return false;
	return true;
}
bool Math::even(const Integer& num)
{
	if (num.number.empty() || num.type != NumSpecialType::None)
		return false;
	return num.number[0] % '\002' == 0;
}
bool Math::noteven(const Integer& num)
{
	if (num.number.empty() || num.type != NumSpecialType::None)
		return false;
	return num.number[0] % '\002' != 0;
}
bool Math::isint(const Fractional& num)
{
	return num.type == NumSpecialType::None ? false : num.afterDot == 0;
}

void Math::roundUp10(Integer& a, Integer& b)
{
	while (a.number[0] % 5 == 0 && b.number[0] % 5 == 0)
	{
		a /= 5;
		b /= 5;
	}
	while (a.number[0] % 2 == 0 && b.number[0] % 2 == 0)
	{
		a /= 2;
		b /= 2;
	}
}

#pragma endregion

#pragma region triganam

Fractional Math::pi(const unsigned int n)
{
	Fractional pi = 0.0;
	Fractional pw = 1;

	Fractional pr1 = 1;
	Fractional pr2;
	Fractional pr3;

	pi.SetMaxSignsAfterDotDiving(n);
	pw.SetMaxSignsAfterDotDiving(n);
	pr1.SetMaxSignsAfterDotDiving(n);
	pr2.SetMaxSignsAfterDotDiving(n);
	pr3.SetMaxSignsAfterDotDiving(n);

	for (int k = 0; k < n; ++k) {
		pr1 = 1;
		pr1 /= pw;

		pr3 = 4;
		pr3 /= (8 * k + 1);
		pr2 = pr3;

		pr3 = 2;
		pr3 /= (8 * k + 4);
		pr2 -= pr3;

		pr3 = 1;
		pr3 /= (8 * k + 5);
		pr2 -= pr3;

		pr3 = 1;
		pr3 /= (8 * k + 6);
		pr2 -= pr3;

		pi += pr1 * pr2;

		pw *= 16;
	}
	pi.EraseAfterMaxAfterDot();
	return pi;
}
Fractional Math::sin(const Fractional& xr, const Fractional& epsilon)
{
	if (xr.type != NumSpecialType::None)
		return Fractional::FabrickNan(xr.negative);
	if (xr.Null())
		return Fractional{};
	Fractional x = xr;
	x %= Math::pi(epsilon.AfterDot()) * 2;
	Fractional res = 0;
	Fractional pr = x;
	int prden = 2;
	while (abs(pr) >= epsilon)
	{
		res += pr;
		pr *= -(x * x) / (prden * (prden + 1));
		prden += 2;
	}
	res.SetMaxSignsAfterDotDiving(epsilon.afterDot);
	res.EraseAfterMaxAfterDot();
	return res;
}
Fractional Math::sin(const Fractional& xr)
{
	return sin(xr, defaultEpsilon);
}
Fractional Math::cos(const Fractional& xr, const Fractional& epsilon)
{
	if (xr.type != NumSpecialType::None)
		return Fractional::FabrickNan(xr.negative);
	if (xr.Null())
		return 1;
	Fractional x = xr;
	x %= pi(epsilon.afterDot) * 2;
	Fractional result = 1.0;
	Fractional term = 1.0;
	int n = 1;
	while (abs(term) >= epsilon) {
		term *= -x * x / ((2 * n) * (2 * n - 1));
		result += term;
		++n;
	}
	result.SetMaxSignsAfterDotDiving(epsilon.afterDot);
	result.EraseAfterMaxAfterDot();
	return result;
}
Fractional Math::cos(const Fractional& xr)
{
	return cos(xr, defaultEpsilon);
}
Fractional Math::tg(const Fractional& xr, const Fractional& epsilon)
{
	return sin(xr, epsilon) / cos(xr, epsilon);
}
Fractional Math::tg(const Fractional& xr)
{
	return tg(xr, defaultEpsilon);
}
Fractional Math::ctg(const Fractional& xr, const Fractional& epsilon)
{
	return cos(xr, epsilon) / sin(xr, epsilon);
}
Fractional Math::ctg(const Fractional& xr)
{
	return ctg(xr, defaultEpsilon);
}

#pragma endregion

#pragma region arctriganam

Fractional Math::arctg(const Fractional& x, const Fractional& epsilon)
{
	if (x.type == NumSpecialType::Nan)
		return x;
	if (x.type == NumSpecialType::Infinity)
		return pi(epsilon.afterDot) / 2;
	if (x < 0)
		return -arctg(-x, epsilon);
	if (x > 1)
		return Math::pi(epsilon.afterDot) / 2 - arctg(1_fb / x, epsilon);
	if (x.Null())
		return Fractional{};
	if (x == 1)
		return pi(epsilon.afterDot) / 4;
	Fractional sum = 0;
	Fractional term = x;
	int n = 1;
	while (Math::abs(term) > epsilon) {
		sum += term;
		term = -term * x * x * (2 * n - 1) / (2 * n + 1);
		n++;
	}
	sum.SetMaxSignsAfterDotDiving(epsilon.afterDot);
	sum.EraseAfterMaxAfterDot();
	return sum;
}
Fractional Math::arctg(const Fractional& x)
{
	return arctg(x, defaultEpsilon);
}
Fractional Math::arcctg(const Fractional& x, const Fractional& epsilon)
{
	return pi(epsilon.afterDot) / 2 - arctg(x, epsilon);
}
Fractional Math::arcctg(const Fractional& x)
{
	return arcctg(x, defaultEpsilon);
}
Fractional Math::arcsin(const Fractional& x, const Fractional& epsilon)
{
	if (x.type != NumSpecialType::None)
		return Fractional::FabrickNan(true);
	if (x > 1 || x < -1)
		return Fractional::FabrickNan(x.negative);
	return Math::arctg(x / sqrt(1_fb - x * x, epsilon), epsilon);
}
Fractional Math::arcsin(const Fractional& x)
{
	return arcsin(x, defaultEpsilon);
}
Fractional Math::arccos(const Fractional& x, const Fractional& epsilon)
{
	if (x.type != NumSpecialType::None)
		return Fractional::FabrickNan(true);
	if (x > 1 || x < -1)
		return Fractional::FabrickNan(x.negative);
	return pi(epsilon.afterDot) / 2 - arcsin(x, epsilon);
}
Fractional Math::arccos(const Fractional& x)
{
	return arccos(x, defaultEpsilon);
}

#pragma endregion

#pragma endregion