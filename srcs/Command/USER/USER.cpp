#include <set>
#include "Server.hpp"
#include "UserDB.hpp"
#include "ChannelDB.hpp"
#include "Message.hpp"
#include <string>

enum {

	RPL_WELCOME = 1,
    ERR_NEEDMOREPARAMS = 461,
    ERR_ALREADYREGISTERED = 462

};

void	HookFunctionUser(const Message& message)
{
	Server&			server = Server::GetInstance();
	UserDB&			userDB = UserDB::GetInstance();
	int				userId = message.GetUserId();

	if (message.GetParameters().size() < 3 || message.GetParameters().at(0).length() == 0)
	{
		userDB.SendErrorMessageToUser("", userId, ERR_NEEDMOREPARAMS);
		return ;
	}
	if (userDB.GetLoginStatus(userId))
	{
		userDB.SendErrorMessageToUser("", userId, ERR_ALREADYREGISTERED);
		return ;
	}

	std::string userName = message.GetParameters().at(0);
	userDB.SetUserName(userId, userName);
	userDB.SendErrorMessageToUser(":welcome!", userId, RPL_WELCOME);
}
