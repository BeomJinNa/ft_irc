#include <sstream>
#include "Message.hpp"

Message::Message(void) {}

Message::Message(const Message& source)
	: mPrefix(source.mPrefix)
	, mCommand(source.mPrefix)
	, mMessage(source.mMessage)
	, mRawMessage(source.mRawMessage) {}

Message&	Message::operator=(const Message& source)
{
	if (this != &source)
	{
		mPrefix = source.mPrefix;
		mCommand = source.mCommand;
		mMessage = source.mMessage;
		mRawMessage = source.mRawMessage;
	}

	return (*this);
}

Message::~Message(void) {}

int					Message::GetUserId(void) const			{ return (mUserId); }
std::string&		Message::GetPrefix(void)				{ return (mPrefix); }
const std::string&	Message::GetPrefix(void) const			{ return (mPrefix); }
std::string&		Message::GetCommand(void)				{ return (mCommand); }
const std::string&	Message::GetCommand(void) const			{ return (mCommand); }
std::string&		Message::GetMessage(void)				{ return (mMessage); }
const std::string&	Message::GetMessage(void) const			{ return (mMessage); }
std::string&		Message::GetRawMessageData(void)		{ return (mRawMessage); }
const std::string&	Message::GetRawMessageData(void) const	{ return (mRawMessage); }

bool	Message::ParseMessage(int userId, std::string& message)
{
	std::istringstream	iss(message);
	std::string			token;

	mRawMessage = message;
	mUserId = userId;
	//prefix가 존재할 경우 파싱
	if (!message.empty() && message[0] == ':')
	{
		std::getline(iss, mPrefix, ' ');
		mPrefix = mPrefix.substr(1);
	}

	//Command 파싱, Command는 필수이므로 없는 경우 즉시 에러 반환
	if (!std::getline(iss, mCommand, ' ')
	 || mCommand.empty() || mCommand[0] == ':')
	{
		ClearData();
		return (false);
	}

	//Command 뒷부분은 모두 mMessage에 저장
	std::getline(iss, mMessage);
	return (true);
}

void	Message::ClearData(void)
{
	mPrefix.clear();
	mCommand.clear();
	mMessage.clear();
	mRawMessage.clear();
}
