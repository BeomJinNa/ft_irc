#include <stdexcept>
#include "ChannelDB.hpp"

namespace
{
	ChannelDB*	TouchInstanceData(ChannelDB* address);
}

ChannelDB::ChannelDB(void)
{
	TouchInstanceData(this);
}

ChannelDB::~ChannelDB(void) {}

ChannelDB& ChannelDB::GetInstance(void)
{
	ChannelDB*	output = TouchInstanceData(NULL);

	if (output == NULL)
		throw std::runtime_error("Channel Database Not Found");
	return (*output);
}

namespace
{
	ChannelDB*	TouchInstanceData(ChannelDB* address)
	{
		static ChannelDB*	ChannelDBGlobal = NULL;

		if (address != NULL)
		{
			ChannelDBGlobal = address;
			return (NULL);
		}

		return (ChannelDBGlobal);
	}
}

void	ChannelDB::DoNothing(void) const {}
ChannelDB::ChannelDB(const ChannelDB& source) { (void)source; }
ChannelDB&	ChannelDB::operator=(const ChannelDB& source)
{ if (this != &source) {} return (*this); }
