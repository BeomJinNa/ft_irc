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

		~Command(void);

		static void RegisterCommand(const std::string& commandName, CommandFunction function);
		static bool	ExecuteCommand(const Message& message);

	private:
		Command(void);
		Command(const Command& source);
		Command&	operator=(const Command& source);

		static std::map<std::string, CommandFunction>	mHookList;
};
#endif
