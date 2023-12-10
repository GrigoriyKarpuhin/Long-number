#ifndef LAB3_LN_H
#define LAB3_LN_H
#include "OwnVector.h"

#include <iostream>

class LN
{
  public:
	LN()
	{
		negate_ = false;
		isNaN_ = false;
	}

	LN(long long value);

	explicit LN(const char* str);

	explicit LN(std::string_view);

	LN(const LN& other);

	LN(LN&& other) noexcept;

	~LN() = default;

	uint operator[](int index) const;

	LN& operator+=(const LN& other);

	LN operator+(const LN& other);

	LN& operator-=(const LN& other);

	LN operator-(const LN& other);

	LN operator-();

	bool operator==(const LN& other);

	bool operator!=(const LN& other);

	bool operator<(const LN& other);

	bool operator>(const LN& other);

	bool operator<=(const LN& other);

	bool operator>=(const LN& other);

	operator bool() const;

	operator long long() const;

	LN& operator=(const LN& other);

	LN& operator*=(const LN& other);

	LN operator*(const LN& other);

	LN& operator/=(const LN& other);

	LN operator/(const LN& other);

	LN& operator%=(const LN& other);

	LN operator%(const LN& other);

	LN operator~();

	friend char* toString(LN number);

  private:
	bool negate_;
	bool isNaN_;
	OwnVector number_;

	static int compare(LN first, LN second);

	void deleteLeadingZeros();

	void multByConst(uint64_t value);

	void addByConst(uint64_t value);

	void divByConst(uint64_t value);
};

LN operator""_ln(const char* str);

char* toString(LN number);

#endif
