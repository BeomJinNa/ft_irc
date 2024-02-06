#include <set>
#include "Server.hpp"
#include "UserDB.hpp"
#include "ChannelDB.hpp"
#include "Message.hpp"
#include <string>

enum {

    ERR_NEEDMOREPARAMS = 461,
    ERR_ALREADYREGISTERED = 462,
    ERR_PASSWDMISMATCH = 464

};

//TODO: enum header

void	HookFunctionPass(const Message& message)
{
	Server&			server = Server::GetInstance();
	int				userId = message.GetUserId();

	if (message.GetParameters().size() == 0) //TODO: Param 개수 0 가능?
	{
		std::string errCode = std::to_string(ERR_NEEDMOREPARAMS);
    	server.SendMessageToClient(userId, errCode.c_str(), errCode.length());
		return;
	}

	std::string		inputPassword = message.GetParameters().at(0);
	UserDB&			userDB = UserDB::GetInstance();

	if (userDB.GetUserIdBySocketId(userId) != -1)
	{
		std::string errCode = std::to_string(ERR_ALREADYREGISTERED);
    	server.SendMessageToClient(userId, errCode.c_str(), errCode.length());
		return;
	}
	if (server.GetServerPassword() != inputPassword)
	{
		std::string errCode = std::to_string(ERR_PASSWDMISMATCH);
    	server.SendMessageToClient(userId, errCode.c_str(), errCode.length());
		return;
	}

	userDB.ConnectUser(userId);
}

//TODO: test!
