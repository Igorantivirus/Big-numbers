#pragma once

class DecVector
{
public:
	DecVector();
	DecVector(size_t size);
	DecVector(const DecVector& other);

	~DecVector();

	DecVector& operator=(const DecVector& other);

	void clear();

	void reserve(size_t size);
	void resize(size_t size);

	void erase(size_t begin, size_t count = 1);

	size_t size() const;
	size_t capacity() const;

	char* begin() const;
	char* end() const;

	bool empty() const;

	char& operator[](size_t size);
	char operator[](size_t size) const;

	char& back();
	char back() const;

	void push_back(char s);
	void push_begin(char s);
	void push_begin_memory(size_t count, char s = '\000');
	void pop_back();
	void pop_begin();

	void push_begin_count(size_t count);

	void reverse();

private:
	char* _dec = nullptr;
	size_t _capacity = 0;
	size_t _size = 0;
};