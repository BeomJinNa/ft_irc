#include <set>
#include <string>
#include "Server.hpp"
#include "UserDB.hpp"
#include "ChannelDB.hpp"
#include "Message.hpp"
#include "ErrorCodes.hpp"
#include "ReplyCodes.hpp"

void	HookFunctionUser(const Message& message)
{
	Server&			server = Server::GetInstance();
	UserDB&			userDB = UserDB::GetInstance();
	int				userId = message.GetUserId();

	if (message.GetParameters().size() < 3 || message.GetParameters().at(0).length() == 0)
	{
		userDB.SendErrorMessageToUser("USER :Not enough parameters", userId, M_ERR_NEEDMOREPARAMS, userId);
		return ;
	}
	if (userDB.GetLoginStatus(userId))
	{
		// userDB.SendErrorMessageToUser(":You may not reregister", userId, M_ERR_ALREADYREGISTERED, userId);
		return ;
	}

	std::string userName = message.GetParameters().at(0);
	userDB.SetUserName(userId, userName); // TODO: add other attributes in User class
	userDB.SendErrorMessageToUser(":Welcome to the " + server.GetHostAddress() + " Network, " + userDB.GetNickName(userId), userId, M_RPL_WELCOME, userId);
}
