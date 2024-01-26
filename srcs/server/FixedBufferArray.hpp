#pragma once
#ifndef FIXEDBUFFERARRAY_HPP
# define FIXEDBUFFERARRAY_HPP

# include "Constant.hpp"

class FixedBufferArray
{
	public:
		FixedBufferArray(void);
		FixedBufferArray(const FixedBufferArray& source);
		FixedBufferArray&	operator=(const FixedBufferArray& source);
		~FixedBufferArray(void);

		char	buffer[M_SERVER_READ_BUFFER_SIZE];
};
#endif
