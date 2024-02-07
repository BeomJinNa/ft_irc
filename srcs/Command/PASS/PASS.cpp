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

void	HookFunctionPass(const Message& message)
{
	Server&			server = Server::GetInstance();
	UserDB&			userDB = UserDB::GetInstance();
	int				userId = message.GetUserId();

	if (message.GetParameters().size() == 0)
	{
		std::string errMsg = std::to_string(ERR_NEEDMOREPARAMS);
		userDB.SendMessageToUser(errMsg, userId);
		return ;
	}
	if (userDB.GetLoginStatus())
	{
		std::string errMsg = std::to_string(ERR_ALREADYREGISTERED);
		userDB.SendMessageToUser(errMsg, userId);
		return ;
	}

	const std::string&	inputPassword = message.GetParameters().at(0);

	if (server.GetServerPassword() != inputPassword)
	{
		std::string errMsg = std::to_string(ERR_PASSWDMISMATCH);
		userDB.SendMessageToUser(errMsg, userId);
		return ;
	}

	userDB.SetLoginStatus(userId, true);
}
