#pragma once
#ifndef COMMAND_HPP
# define COMMAND_HPP

# include <map>
# include <string>

class Server;
class Message;

class Command
{
	public:
		typedef void	(*CommandFunction)(const Message& message);

		Command(const std::string& command);
		Command(const Command& source);
		Command&	operator=(const Command& source);
		~Command(void);

		std::string	GetCommand(void) const;
		static bool	ExecuteCommand(const Message& message);

	private:
		Command(void);
		std::string										mCommand;
		static std::map<std::string, CommandFunction>	mHookList;
};
#endif
