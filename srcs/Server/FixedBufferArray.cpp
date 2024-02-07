#include <cstddef>
#include "FixedBufferArray.hpp"

FixedBufferArray::FixedBufferArray(void)
{
	for (size_t i = 0; i < 1024; ++i)
		buffer[i] = '\0';
}

FixedBufferArray::FixedBufferArray(const FixedBufferArray& source)
{
	for (size_t i = 0; i < 1024; ++i)
		buffer[i] = source.buffer[i];
}

FixedBufferArray&	FixedBufferArray::operator=(const FixedBufferArray& source)
{
	if (this != &source)
	{
		for (size_t i = 0; i < 1024; ++i)
			buffer[i] = source.buffer[i];
	}

	return (*this);
}

FixedBufferArray::~FixedBufferArray(void) {}
