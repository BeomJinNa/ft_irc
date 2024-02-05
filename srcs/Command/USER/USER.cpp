#include <set>
#include "Server.hpp"
#include "UserDB.hpp"
#include "ChannelDB.hpp"
#include "Message.hpp"
#include <string>

enum {

    //PASS에 있는 애들
    ERR_NEEDMOREPARAMS = 461,
    ERR_ALREADYREGISTERED = 462

};

//TODO: parsing 고쳐야함, 상속 고려?
/*

	Map<std::string, Command*> makeCommandMap
	{
		cmdMap.insert({"join", Join});
		cmdMap.insert({"pass", Pass});
		...

		return cmdMap;
	}

	Command* newCommand(std::string comm, std::string rawMsg)
	{

	}

	readHandler(socketFd, buffer)
	{

		std::string rawMsg = buffer;
		std::string comm = rawMsg.at[0];

		Command* command = newCommand(comm, rawMsg)
		command[i].parse(); // 생성자 안에서 호출?
		command[i].execute();
	}

bool	Command::ExecuteCommand(const Message& message)
{
	std::map<std::string, Command>it = cmdMap.find("/join");

	if (it == cmdMap.end())
		return false;

	try
	{
		(*it->second).execute(rawMsg); // call (overridden) parse() then execute
	}
	catch (std::exception& e)
	{
		std::cerr << e.what();
	}

	return true;
}



*/
void	HookFunctionUser(const Message& message)
{

}
