#include "OwnVector.h"

#include <iostream>

typedef uint32_t uint;
using namespace std;
uint& OwnVector::operator[](int index) const
{
	return array_[index];
}

uint& OwnVector::operator[](int index)
{
	return array_[index];
}

OwnVector& OwnVector::operator=(const OwnVector& other)
{
	delete[] array_;
	array_ = new uint[other.capacity_];
	for (int i = 0; i < other.size_; ++i)
	{
		array_[i] = other.array_[i];
	}
	size_ = other.size_;
	capacity_ = other.capacity_;
	return *this;
}

void OwnVector::pushBack(uint value)
{
	if (size_ == capacity_)
	{
		resize(size_ * 2 + 1);
	}
	array_[size_++] = value;
}

void OwnVector::resize(uint newSize)
{
	if (newSize > capacity_)
	{
		capacity_ = newSize;
		uint* newArray = new uint[capacity_];
		if (array_ != nullptr)
		{
			for (int i = 0; i < size_; ++i)
			{
				newArray[i] = array_[i];
			}
			delete[] array_;
		}
		array_ = newArray;
	}
}

void OwnVector::newSize(uint newSize)
{
	if (newSize > capacity_)
	{
		resize(newSize);
	}
	for (int i = size_; i < newSize; ++i)
	{
		array_[i] = 0;
	}
	size_ = newSize;
}

void OwnVector::popBack()
{
	--size_;
	if (size_ * 4 <= capacity_)
	{
		resize(size_ * 2);
	}
}

void OwnVector::clear()
{
	delete[] array_;
	array_ = nullptr;
	size_ = 0;
	capacity_ = 0;
}

void OwnVector::insert(uint value, uint index, uint addSize)
{
	uint* newArray = new uint[capacity_ + addSize];
	for (int i = 0; i < size_; ++i)
	{
		newArray[i + addSize] = array_[i];
	}
	delete[] array_;
	array_ = newArray;
	capacity_ += addSize;
	size_ += addSize;
	for (int i = index; i < index + addSize; ++i)
	{
		array_[i] = value;
	}
}
