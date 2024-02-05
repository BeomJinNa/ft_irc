#include <string>
#include "Command.hpp"
#include "Message.hpp"

std::map<std::string, Command::CommandFunction> Command::mHookList;

Command::Command(void) {}
Command::Command(const std::string& command) : mCommand(command) {}
Command::Command(const Command& source) : mCommand(source.mCommand) {}
Command&	Command::operator=(const Command& source)
{ if (this != &source) { mCommand = source.mCommand; } return (*this); }
Command::~Command(void) {}

std::string	Command::GetCommand(void) const { return (mCommand); }

bool	Command::ExecuteCommand(const Message& message)
{
	std::map<std::string, CommandFunction>::iterator	it
		= mHookList.find(message.GetCommand());

	if (it != mHookList.end())
	{
		try { (*it->second)(message); } catch (...) {}
		return (false);
	}
	return (true);
}
