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

		int								GetUserId(void) const;
		std::string&					GetPrefix(void);
		const std::string&				GetPrefix(void) const;
		std::string&					GetCommand(void);
		const std::string&				GetCommand(void) const;
		std::vector<std::string>&		GetParameters(void);
		const std::vector<std::string>&	GetParameters(void) const;
		std::string&					GetTrailing(void);
		const std::string&				GetTrailing(void) const;
		std::string&					GetRawMessageData(void);
		const std::string&				GetRawMessageData(void) const;

		bool							ParseMessage(int clientFd, std::string& message);

	private:
		void	ClearData(void);

		int							mUserId;
		std::string					mPrefix;
		std::string					mCommand;
		std::vector<std::string>	mParameters;
		std::string					mTrailing;
		std::string					mRawMessage;
};
#endif
