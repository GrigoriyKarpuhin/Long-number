#include "LN.h"

#include "return_codes.h"

#include <algorithm>
#include <climits>
#include <cmath>
#include <cstring>
#include <iostream>

using namespace std;
typedef uint32_t uint;

#define BIT_SIZE 32

LN::LN(long long value)
{
	negate_ = false;
	isNaN_ = false;
	if (value == 0)
	{
		number_.pushBack(0);
		return;
	}
	uint64_t temp;
	if (value < 0)
	{
		if (value == LLONG_MIN)
		{
			temp = LLONG_MAX;
			temp += 1;
			negate_ = true;
		}
		else
		{
			temp = -value;
			negate_ = true;
		}
	}
	else
	{
		temp = value;
	}
	while (temp > 0)
	{
		number_.pushBack(temp);
		temp >>= BIT_SIZE;
	}
}

LN::LN(const char* str)
{
	negate_ = false;
	isNaN_ = false;
	while (*str == ' ')
	{
		++str;
	}
	if (str[0] == '-')
	{
		negate_ = true;
	}
	else
	{
		negate_ = false;
	}
	int temp = (negate_ ? 1 : 0);
	while (str[temp] == '0' && str[temp++] != '\0')
	{
		++temp;
	}
	for (int i = temp; str[i] != '\0'; i++)
	{
		multByConst(16);
		if (str[i] >= '0' && str[i] <= '9')
		{
			addByConst(str[i] - '0');
		}
		else if (str[i] >= 'A' && str[i] <= 'F')
		{
			addByConst(str[i] - 'A' + 10);
		}
		else if (str[i] >= 'a' && str[i] <= 'f')
		{
			addByConst(str[i] - 'a' + 10);
		}
		else
		{
			isNaN_ = true;
			throw invalid_argument("Invalid argument");
		}
	}
}

LN::LN(string_view str)
{
	negate_ = false;
	isNaN_ = false;
	while (str.front() == ' ')
	{
		str.remove_prefix(1);
	}
	if (str.front() == '-')
	{
		negate_ = true;
		str.remove_prefix(1);
	}
	else
	{
		negate_ = false;
	}
	while (str.front() == '0')
	{
		str.remove_prefix(1);
	}
	while (str.length() > 0)
	{
		multByConst(16);
		if (str.front() >= '0' && str.front() <= '9')
		{
			addByConst(str.front() - '0');
		}
		else if (str.front() >= 'A' && str.front() <= 'F')
		{
			addByConst(str.front() - 'A' + 10);
		}
		else if (str.front() >= 'a' && str.front() <= 'f')
		{
			addByConst(str.front() - 'a' + 10);
		}
		else
		{
			isNaN_ = true;
			throw invalid_argument("Invalid argument");
		}
		str.remove_prefix(1);
	}
}

LN::LN(const LN& other)
{
	negate_ = other.negate_;
	isNaN_ = other.isNaN_;
	number_ = other.number_;
}

LN::LN(LN&& other) noexcept
{
	negate_ = other.negate_;
	isNaN_ = other.isNaN_;
	number_ = move(other.number_);
}

uint LN::operator[](int index) const
{
	return number_[index];
}

LN& LN::operator+=(const LN& other)
{
	if (isNaN_ || other.isNaN_)
	{
		isNaN_ = true;
		return *this;
	}
	if (negate_ ^ other.negate_)
	{
		if (compare(*this, other) >= 0)
		{
			if (negate_)
			{
				negate_ = false;
			}
			else
			{
				negate_ = true;
			}
			this->operator-=(other);
		}
		else
		{
			if (negate_)
			{
				negate_ = false;
			}
			else
			{
				negate_ = true;
			}
			LN temp = other;
			temp.operator-=(*this);
			*this = temp;
		}
	}
	else
	{
		if (negate_ && other.negate_)
		{
			negate_ = true;
		}
		uint shift = 0;
		number_.newSize(max(number_.getSize(), other.number_.getSize()));
		for (int i = 0; i < max(number_.getSize(), other.number_.getSize()); ++i)
		{
			uint64_t first = (i < number_.getSize() ? number_[i] : 0);
			uint64_t second = (i < other.number_.getSize() ? other.number_[i] : 0);
			uint64_t temp = first + second + shift;
			number_[i] = temp;
			shift = temp >> BIT_SIZE;
		}
		number_.pushBack(shift);
		deleteLeadingZeros();
	}
	return *this;
}

LN LN::operator+(const LN& other)
{
	LN result = *this;
	result += other;
	return result;
}

LN& LN::operator-=(const LN& other)
{
	if (isNaN_ || other.isNaN_)
	{
		isNaN_ = true;
		return *this;
	}
	if (negate_ ^ other.negate_)
	{
		if (negate_)
		{
			negate_ = false;
		}
		else
		{
			negate_ = true;
		}
		this->operator+=(other);
		negate_ = !negate_;
	}
	else
	{
		if (compare(*this, other) >= 0)
		{
			uint shift = 0;
			number_.newSize(max(number_.getSize(), other.number_.getSize()) + 1);
			for (int i = 0; i < number_.getSize(); ++i)
			{
				uint64_t first = (i < number_.getSize() ? number_[i] : 0);
				uint64_t second = (i < other.number_.getSize() ? other.number_[i] : 0);
				uint64_t res = ((uint64_t)1 << BIT_SIZE) + first;
				res -= second + shift;
				number_[i] = res;
				shift = (res >> BIT_SIZE) ^ 1;
			}
			deleteLeadingZeros();
		}
		else
		{
			LN temp = other;
			temp.operator-=(*this);
			*this = temp;
			if (negate_)
			{
				negate_ = false;
			}
			else
			{
				negate_ = true;
			}
		}
	}
	return *this;
}

LN LN::operator-(const LN& other)
{
	LN result = *this;
	result -= other;
	return result;
}

LN LN::operator-()
{
	LN result = *this;
	if (result.number_.getSize() == 1 && result.number_[0] == 0)
	{
		return result;
	}
	if (result.negate_)
	{
		result.negate_ = false;
	}
	else
	{
		result.negate_ = true;
	}
	return result;
}

bool LN::operator==(const LN& other)
{
	if (isNaN_ || other.isNaN_)
	{
		return false;
	}
	if (negate_ != other.negate_)
	{
		return false;
	}
	return compare(*this, other) == 0;
}

bool LN::operator!=(const LN& other)
{
	return !(*this == other);
}

bool LN::operator<(const LN& other)
{
	if (isNaN_ || other.isNaN_)
	{
		return false;
	}
	if (negate_ && !other.negate_)
	{
		return true;
	}
	else if (!negate_ && other.negate_)
	{
		return false;
	}
	else if (negate_ && other.negate_)
	{
		return compare(*this, other) > 0;
	}
	else
	{
		return compare(*this, other) < 0;
	}
}

bool LN::operator>(const LN& other)
{
	if (isNaN_ || other.isNaN_)
	{
		return false;
	}
	if (negate_ && !other.negate_)
	{
		return false;
	}
	else if (!negate_ && other.negate_)
	{
		return true;
	}
	else if (negate_ && other.negate_)
	{
		return compare(*this, other) < 0;
	}
	else
	{
		return compare(*this, other) > 0;
	}
}

bool LN::operator<=(const LN& other)
{
	return !(*this > other);
}

bool LN::operator>=(const LN& other)
{
	return !(*this < other);
}

LN::operator long long() const
{
	try
	{
		if (number_.getSize() > 2 || (!negate_ && number_[1] == 1 << (BIT_SIZE - 1)) ||
			(negate_ && number_[1] == 1 << (BIT_SIZE - 1) && number_[0] > 0))
		{
			throw overflow_error("Overflow");
		}
		long long result = 0;
		for (int i = number_.getSize() - 1; i >= 0; i--)
		{
			result <<= BIT_SIZE;
			result += number_[i];
		}
		if (negate_)
		{
			result = -result;
		}
		return result;
	} catch (...)
	{
		cerr << "Overflow for data type: 'long long'" << endl;
		return ERROR_DATA_INVALID;
	}
}

LN::operator bool() const
{
	if (number_.getSize() == 1 && number_[0] == 0)
	{
		return false;
	}
	return true;
}

LN& LN::operator=(const LN& other)
{
	number_ = other.number_;
	negate_ = other.negate_;
	isNaN_ = other.isNaN_;
	return *this;
}

LN& LN::operator*=(const LN& other)
{
	number_.insert(0, 0, other.number_.getSize());
	uint64_t shift;
	uint64_t res;
	for (int i = 0; i < number_.getSize() - other.number_.getSize(); ++i)
	{
		shift = 0;
		res = number_[i + other.number_.getSize()];
		uint64_t temp;
		for (int j = 0; j < other.number_.getSize(); ++j)
		{
			temp = res * other.number_[j] + shift + number_[i + j];
			number_[i + j] = temp;
			shift = temp >> BIT_SIZE;
		}
		number_[i + other.number_.getSize()] = shift;
	}
	deleteLeadingZeros();
	if (negate_ == other.negate_)
	{
		negate_ = false;
	}
	else
	{
		negate_ = true;
	}
	return *this;
}

LN LN::operator*(const LN& other)
{
	LN result = *this;
	result *= other;
	return result;
}

LN& LN::operator/=(const LN& other)
{
	if (other.number_.getSize() == 1 && other.number_[0] == 0)
	{
		isNaN_ = true;
		return *this;
	}
	if (compare(*this, other) < 0)
	{
		number_.clear();
		number_.pushBack(0);
		negate_ = false;
		return *this;
	}
	if (compare(*this, other) == 0)
	{
		number_.clear();
		number_.pushBack(1);
		negate_ = negate_ ^ other.negate_;
		return *this;
	}
	negate_ = (other.negate_ ? !negate_ : negate_);
	LN first = *this;
	LN second = other;
	first.negate_ = false;
	second.negate_ = false;
	uint normal = 0;
	while (second.number_.back() < 1 << (BIT_SIZE - 1))
	{
		second.multByConst(2);
		++normal;
	}
	first.multByConst(1 << normal);
	LN result = 0;
	LN temp1 = second;
	uint diff = first.number_.getSize() - second.number_.getSize();
	temp1.number_.insert(0, 0, diff);
	result.number_.newSize(diff + 1);
	if (first >= temp1)
	{
		result.number_[diff] = 1;
		first -= temp1;
	}
	else
	{
		result.number_[diff] = 0;
	}
	for (size_t i = diff; i > 0; --i)
	{
		uint64_t r =
			((uint64_t)(first.number_.back()) * ((uint64_t)1 << BIT_SIZE) + first.number_[first.number_.getSize() - 2]) /
			second.number_.back();
		result.number_[i - 1] = min(r, (uint64_t)(((uint64_t)1 << BIT_SIZE) - 1));
		temp1 = second;
		temp1.number_.insert(0, 0, i - 1);
		LN temp2 = temp1;
		temp2.multByConst(result.number_[i - 1]);
		first -= temp2;
		while (first < LN())
		{
			--result.number_[i - 1];
			first += temp1;
		}
	}
	first.divByConst((uint64_t)1 << BIT_SIZE);
	result.deleteLeadingZeros();
	first.deleteLeadingZeros();
	*this = result;
	deleteLeadingZeros();
	return *this;
}

LN LN::operator/(const LN& other)
{
	LN result = *this;
	result /= other;
	return result;
}

LN& LN::operator%=(const LN& other)
{
	if (other.number_.getSize() == 1 && other.number_[0] == 0)
	{
		isNaN_ = true;
		return *this;
	}
	if (compare(*this, other) < 0)
	{
		return *this;
	}
	LN result;
	result.number_.clear();
	result.number_.pushBack(0);
	result.number_.insert(0, 0, number_.getSize() - other.number_.getSize() + 1);
	LN temp;
	temp.number_.clear();
	temp.number_.pushBack(0);
	temp.number_.insert(0, 0, number_.getSize() - other.number_.getSize() + 1);
	for (int i = number_.getSize() - other.number_.getSize(); i >= 0; --i)
	{
		temp.number_[i] = number_[i + other.number_.getSize() - 1];
		while (temp >= other)
		{
			temp -= other;
			result.number_[i]++;
		}
	}
	result.negate_ = negate_ != other.negate_;
	result.deleteLeadingZeros();
	*this = temp;
	return *this;
}

LN LN::operator%(const LN& other)
{
	LN result = *this;
	result %= other;
	return result;
}

LN LN::operator~()
{
	LN result = *this;
	if (result.negate_)
	{
		result.isNaN_ = true;
		return result;
	}
	LN temp = result;
	while (temp * temp > result)
	{
		temp = (temp + result / temp) / 2_ln;
	}
	return temp;
}

LN operator""_ln(const char* str)
{
	return LN(str);
}

int LN::compare(LN first, LN second)
{
	if (first.number_.getSize() > second.number_.getSize())
	{
		return 1;
	}
	else if (first.number_.getSize() < second.number_.getSize())
	{
		return -1;
	}
	else
	{
		for (int i = first.number_.getSize() - 1; i >= 0; --i)
		{
			if (first.number_[i] > second.number_[i])
			{
				return 1;
			}
			else if (first.number_[i] < second.number_[i])
			{
				return -1;
			}
		}
		return 0;
	}
}

void LN::deleteLeadingZeros()
{
	while (number_.getSize() > 1 && number_[number_.getSize() - 1] == 0)
	{
		number_.popBack();
	}
}

void LN::multByConst(uint64_t value)
{
	uint64_t shift = 0;
	for (int i = 0; i < number_.getSize(); ++i)
	{
		uint64_t res = number_[i] * value + shift;
		number_[i] = res;
		shift = res >> BIT_SIZE;
	}
	if (shift > 0)
	{
		number_.pushBack(shift);
	}
}

void LN::addByConst(uint64_t value)
{
	uint64_t shift = value;
	for (int i = 0; i < number_.getSize(); ++i)
	{
		uint64_t res = number_[i] + shift;
		number_[i] = res;
		shift = res >> BIT_SIZE;
	}
	if (shift > 0)
	{
		number_.pushBack(shift);
	}
}

void LN::divByConst(uint64_t value)
{
	uint64_t shift = 0;
	for (int i = (uint64_t)number_.getSize() - 1; i >= 0; --i)
	{
		uint64_t res = ((uint64_t)number_[i] + (shift << BIT_SIZE)) / value;
		shift = ((uint64_t)number_[i] + (shift << BIT_SIZE)) % value;
		number_[i] = res;
	}
	deleteLeadingZeros();
}

char* toString(LN number)
{
	if (number.isNaN_)
	{
		return "NaN";
	}
	if (number.number_.getSize() == 1 && number.number_[0] == 0)
	{
		return "0";
	}
	char* result = new char[number.number_.getSize() * 16 + 1];
	result[0] = '\0';
	LN temp = number;
	while (temp.number_.getSize() > 1 || temp.number_[0] > 0)
	{
		uint64_t r = temp.number_[0] % 16;
		temp.divByConst(16);
		char* tempStr = new char[2];
		if (r < 10)
		{
			tempStr[0] = (char)(r + '0');
		}
		else
		{
			tempStr[0] = (char)(r - 10 + 'A');
		}
		tempStr[1] = '\0';
		strcat(result, tempStr);
		delete[] tempStr;
	}
	if (number.negate_)
	{
		char* tempStr = new char[2];
		tempStr[0] = '-';
		tempStr[1] = '\0';
		strcat(result, tempStr);
		delete[] tempStr;
	}
	reverse(result, result + strlen(result));
	return result;
}