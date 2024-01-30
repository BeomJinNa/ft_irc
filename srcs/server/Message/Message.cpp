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
	if (!message.empty() && message[0] == ':')
	{
		std::getline(iss, mPrefix, ' ');
		mPrefix = mPrefix.substr(1);
	}

	if (!std::getline(iss, mCommand, ' '))
	{
		mPrefix.clear();
		mCommand.clear();
		mParameters.clear();
		mTrailing.clear();
		mRawMessage.clear();
		return (false);
	}

	while (std::getline(iss, token, ' '))
	{
		if (!token.empty())
		{
			if (token[0] == ':')
			{
				std::getline(iss, mTrailing);
				mTrailing = token.substr(1) + " " + mTrailing;
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
