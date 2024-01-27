#pragma once
#ifndef CHANNELDB_HPP
# define CHANNELDB_HPP

# include <map>

class Channel;

class ChannelDB
{
	public:
		typedef std::map<std::string, Channel>	DB;

		ChannelDB(void);
		~ChannelDB(void);

		void				DoNothing(void) const;

		static ChannelDB&	GetInstance(void);
	private:
		ChannelDB(const ChannelDB& source);
		ChannelDB&	operator=(const ChannelDB& source);

		DB	mDatabase;
};
#endif
