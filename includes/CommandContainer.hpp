#pragma once
#ifndef COMMANDCONTAINER_HPP
# define COMMANDCONTAINER_HPP

#include <string>
#include <set>

class Command;

class CommandContainer
{
	public:
		typedef std::set<const std::string>	DB;

		CommandContainer(void);
		CommandContainer(const CommandContainer& source);
		CommandContainer&	operator=(const CommandContainer& source);
		~CommandContainer(void);

		bool	SetCommand(const std::string& command);

	private:
		DB		mCommandList;
		Command	*mCommand;

		void	createClassPASS(void);
};
#endif
