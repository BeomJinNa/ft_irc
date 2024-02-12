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
	if (userDB.IsUserAuthorized(userId))
	{
		userDB.SendErrorMessageToUser(":You may not reregister", userId, M_ERR_ALREADYREGISTRED, userId);
		return ;
	}

	std::string userName = message.GetParameters().at(0);
	std::string hostAddress = message.GetParameters().at(2);
	userDB.SetUserName(userId, userName); // TODO: add other attributes in User class
	server.SetHostAddress(hostAddress);

	const std::string&	nickname = UserDB::GetInstance().GetNickName(userId);

	userDB.SendErrorMessageToUser(nickname + " :Welcome to the " + server.GetHostAddress() + " Network, " + nickname, userId, M_RPL_WELCOME, userId);
}
