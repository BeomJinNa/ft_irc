#pragma once
#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include <string>

class Message
{
	public:
		Message(void);
		Message(const Message& source);
		Message&	operator=(const Message& source);
		~Message(void);

		int					GetUserId(void) const;
		std::string&		GetPrefix(void);
		const std::string&	GetPrefix(void) const;
		std::string&		GetCommand(void);
		const std::string&	GetCommand(void) const;
		std::string&		GetMessage(void);
		const std::string&	GetMessage(void) const;
		std::string&		GetRawMessageData(void);
		const std::string&	GetRawMessageData(void) const;

		bool				ParseMessage(int clientFd, std::string& message);

	private:
		void	ClearData(void);

		int			mUserId;
		std::string	mPrefix;
		std::string	mCommand;
		std::string	mMessage;
		std::string	mRawMessage;
};
#endif
