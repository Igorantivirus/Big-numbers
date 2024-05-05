#include "DecVector.hpp"

#include<algorithm>

DecVector::DecVector()
{
	resize(1);
}
DecVector::DecVector(size_t size)
{
	resize(size);
}
DecVector::DecVector(const DecVector& other)
{
	reserve(other._size);
	for (size_t i = 0; i < other._size; i++)
		_dec[i] = other._dec[i];
	_size = other._size;
}

DecVector::~DecVector()
{
	if (_dec != nullptr)
		delete[] _dec;
	_dec = nullptr;
	_capacity = 0;
	_size = 0;
}

DecVector& DecVector::operator=(const DecVector& other)
{
	if (this == &other)
		return *this;
	clear();
	reserve(other._size);
	for (size_t i = 0; i < other._size; i++)
		_dec[i] = other._dec[i];
	_size = other._size;
	return *this;
}

void DecVector::clear()
{
	for (int i = 0; i < _size; i++)
		_dec[i] = '\000';
	_size = 0;
}

void DecVector::reserve(size_t newSize)
{
	if (newSize < _capacity)
		return;
	newSize = std::max(static_cast<size_t>(_capacity * 1.5), newSize + 1);
	char* pr = new char[newSize] {};
	if (_dec != nullptr)
	{
		for (size_t i = 0; i < _size; i++)
			pr[i] = _dec[i];
		delete[] _dec;
	}
	_dec = pr;
	_capacity = newSize;
}
void DecVector::resize(size_t newSize)
{
	reserve(newSize);
	for (size_t i = std::min(_size, newSize); i < std::max(_size, newSize); i++)
		*(_dec + i) = '\000';
	_size = newSize;
}

void DecVector::erase(size_t begin, size_t count)
{
	if (count == 0)
		return;
	if ((count - begin) >= _size)
		return clear();
	for (size_t i = begin + count; i < _size; i++)
		_dec[i - count] = _dec[i];
	if(begin < _size)
		_size -= count;
	for (size_t i = _size; i < _capacity; i++)
		_dec[i] = '\000';
}

size_t DecVector::size() const
{
	return _size;
}
size_t DecVector::capacity() const
{
	return _capacity;
}

char* DecVector::begin() const
{
	return _dec;
}
char* DecVector::end() const
{
	return _dec + _size;
}

bool DecVector::empty() const
{
	return _size == 0;
}

char& DecVector::operator[](size_t index)
{
	reserve(index + 1);
	return *(_dec + index);
}
char DecVector::operator[](size_t index) const
{
	if (index >= _capacity)
		return '\000';
	return *(_dec + index);
}

char& DecVector::back()
{
	reserve(1);
	return *(_dec + _size - 1);
}
char DecVector::back() const
{
	if (_size == 0)
		return '\000';
	return *(_dec + _size - 1);
}

void DecVector::push_back(char s)
{
	resize(_size + 1);
	_dec[_size - 1] = s;
}
void DecVector::push_begin(char s)
{
	reserve(_size + 1);
	for (size_t i = _size; i > 0; i--)
		_dec[i] = _dec[i - 1];
	_dec[0] = s;
	_size++;
}
void DecVector::push_begin_memory(size_t count, char s)
{
	if (count == 0)
		return;
	reserve(_size + count);
	for (size_t i = _size; i > 0; i--)
		_dec[i + count - 1] = _dec[i - 1];
	for (size_t i = 0; i < count; i++)
		_dec[i] = s;
	_size += count;
}
void DecVector::pop_back()
{
	if (_size == 0 || _dec == nullptr)
		return;
	_dec[_size--] = '\0';
}
void DecVector::pop_begin()
{
	for (size_t i = 0; i < _size; i++)
		_dec[i] = _dec[i + 1];
	_size--;
}

void DecVector::push_begin_count(size_t count)
{
	reserve(_size + count);
	for (size_t i = _size; i > 0; --i)
	{
		_dec[i + count - 1] = _dec[i - 1];
		_dec[i - 1] = '\000';
	}
	_size += count;
}


void DecVector::reverse()
{
	char pr;
	for (size_t i = 0; i < _size / 2; i++)
	{
		pr = _dec[i];
		_dec[i] = _dec[_size - i - 1];
		_dec[_size - i - 1] = pr;
	}
}