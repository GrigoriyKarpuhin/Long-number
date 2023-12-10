#ifndef LAB3_OWNVECTOR_H
#define LAB3_OWNVECTOR_H
#include <cstdint>
#include <iostream>

typedef uint32_t uint;

class OwnVector
{
  public:
	OwnVector() : array_(nullptr), size_(0), capacity_(0) {}

	~OwnVector() = default;

	uint& operator[](int index) const;

	uint& operator[](int index);

	OwnVector& operator=(const OwnVector& other);

	uint getSize() const { return size_; }

	void pushBack(uint value);

	void resize(uint newSize);

	void newSize(uint newSize);

	void popBack();

	void clear();

	void insert(uint value, uint index, uint addSize);

	uint back() const { return array_[size_ - 1]; }

  private:
	uint* array_;
	uint size_;
	uint capacity_;
};

#endif
