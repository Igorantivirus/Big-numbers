#pragma once

#include"BigNumbers.hpp"

class Math
{
public:

	static Integer min(const Integer& a, const Integer& b);
	static Integer max(const Integer& a, const Integer& b);

	static Fractional min(const Fractional& a, const Fractional& b);
	static Fractional max(const Fractional& a, const Fractional& b);

	static Integer abs(const Integer& val);
	static Fractional abs(const Fractional& val);

	static Fractional round(const Fractional& x, size_t afterDot);
	static Fractional round(const Fractional& x);
	static Fractional floor(const Fractional& x);

	static Fractional modf(const Fractional a, Fractional* frac);

	static Fractional pow(const Integer& a, const Integer& b);
	static Fractional pow(const Fractional& a, const Integer& b);
	static Fractional pow(const Integer& a, const Fractional& b);
	static Fractional pow(const Fractional& a, const Fractional& b);
	static Fractional pow(const Fractional& a, const Fractional& b, const Fractional& epsilon);

	static Fractional nroot(const Fractional& a, const Integer& n, const Fractional& epsilon);
	static Fractional nroot(const Fractional& a, const Integer& n);
	static Fractional nroot(const Integer& a, const Integer& n);
	static Fractional nroot(const Fractional& a, const Fractional& n, const Fractional& epsilon);
	static Fractional nroot(const Fractional& a, const Fractional& n);
	static Fractional nroot(const Integer& a, const Fractional& n);

	static Fractional sqrt(const Fractional& x, const Fractional& epsilon);
	static Fractional sqrt(const Fractional& x);
	static Fractional sqrt(const Integer& x);

	static Fractional exp(const Fractional& x, const Fractional& epsilon);
	static Fractional exp(const Integer& x);
	static Fractional exp(const Fractional& x);

	/*Логарифм числа 'z' по основанию числа 'e'
	Алгоритм на основе ряда Тейлора
	The logarithm of the number 'z' based on 'e'
	An algorithm based on the Taylor series
	*/
	static Fractional ln(const Fractional& z, const Fractional& epsilon);
	static Fractional ln(const Fractional& x);
	static Fractional ln(const Integer& x);

	static Integer fact(const Integer& a);

	static bool prime(const Integer& num);
	static bool even(const Integer& num);
	static bool noteven(const Integer& num);
	static bool isint(const Fractional& num);

	static Fractional pi(const unsigned int n = 5);
	static Fractional sin(const Fractional& xr, const Fractional& epsilon);
	static Fractional sin(const Fractional& xr);
	static Fractional cos(const Fractional& xr, const Fractional& epsilon);
	static Fractional cos(const Fractional& xr);
	static Fractional tg(const Fractional& xr, const Fractional& epsilon);
	static Fractional tg(const Fractional& xr);
	static Fractional ctg(const Fractional& xr, const Fractional& epsilon);
	static Fractional ctg(const Fractional& xr);

	static Fractional arctg(const Fractional& x, const Fractional& epsilon);
	static Fractional arctg(const Fractional& x);
	static Fractional arcctg(const Fractional& x, const Fractional& epsilon);
	static Fractional arcctg(const Fractional& x);
	static Fractional arcsin(const Fractional& x, const Fractional& epsilon);
	static Fractional arcsin(const Fractional& x);
	static Fractional arccos(const Fractional& x, const Fractional& epsilon);
	static Fractional arccos(const Fractional& x);

	static Fractional GetEpsolon();
	static void SetEpsilon(const Fractional& val);

private:
	static Fractional defaultEpsilon;

	static void roundUp10(Integer& a, Integer& b);
};

