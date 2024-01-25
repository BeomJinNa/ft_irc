#pragma once
#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include <string>
# include <vector>

class Message
{
	public:
		Message(void);
		Message(const Message& source);
		Message&	operator=(const Message& source);
		~Message(void);

		std::string&				GetPrefix(void);
		std::string&				GetCommand(void);
		std::vector<std::string>&	GetParameters(void);
		std::string&				GetTrailing(void);
		bool						ParseMessage(std::string& message);

	private:
		std::string					mPrefix;
		std::string					mCommand;
		std::vector<std::string>	mParameters;
		std::string					mTrailing;
};
#endif
