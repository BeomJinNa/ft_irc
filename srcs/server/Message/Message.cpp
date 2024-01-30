#include <sstream>
#include "Message.hpp"

Message::Message(void) {}

Message::Message(const Message& source)
	: mPrefix(source.mPrefix)
	, mCommand(source.mPrefix)
	, mParameters(source.mParameters)
	, mTrailing(source.mTrailing) {}

Message&	Message::operator=(const Message& source)
{
	if (this != &source)
	{
		mPrefix = source.mPrefix;
		mCommand = source.mCommand;
		mParameters = source.mParameters;
		mTrailing = source.mTrailing;
	}

	return (*this);
}

Message::~Message(void) {}

int								Message::GetUserId(void) const			{ return (mUserId); }
std::string&					Message::GetPrefix(void)				{ return (mPrefix); }
const std::string&				Message::GetPrefix(void) const			{ return (mPrefix); }
std::string&					Message::GetCommand(void)				{ return (mCommand); }
const std::string&				Message::GetCommand(void) const			{ return (mCommand); }
std::vector<std::string>&		Message::GetParameters(void)			{ return (mParameters); }
const std::vector<std::string>&	Message::GetParameters(void) const		{ return (mParameters); }
std::string&					Message::GetTrailing(void)				{ return (mTrailing); }
const std::string&				Message::GetTrailing(void) const		{ return (mTrailing); }
std::string&					Message::GetRawMessageData(void)		{ return (mRawMessage); }
const std::string&				Message::GetRawMessageData(void) const	{ return (mRawMessage); }

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

	//parameters 파싱
	//콜론`:`으로 시작하는 토큰 발견시 줄 끝까지 trailing으로 처리 후 종료
	while (std::getline(iss, token, ' '))
	{
		if (!token.empty())
		{
			if (token[0] == ':')
			{
				std::getline(iss, mTrailing);
				mTrailing = token.substr(1);
				break;
			}
			else
			{
				mParameters.push_back(token);
			}
		}
	}

	return (true);
}

void	Message::ClearData(void)
{
	mPrefix.clear();
	mCommand.clear();
	mParameters.clear();
	mTrailing.clear();
	mRawMessage.clear();
}
