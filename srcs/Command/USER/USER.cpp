#include <string>
#include "ErrorCodes.hpp"
#include "Message.hpp"
#include "ReplyCodes.hpp"
#include "Server.hpp"
#include "UserDB.hpp"

void	HookFunctionUser(const Message& message)
{
	UserDB&	userDB = UserDB::GetInstance();
	int		userId = message.GetUserId();

	if (message.GetParameters().size() < 3
	 || message.GetParameters().at(0).length() == 0)
	{
		userDB.SendErrorMessageToUser("USER :Not enough parameters", userId,
									  M_ERR_NEEDMOREPARAMS, userId);
		return ;
	}
	if (userDB.IsUserAuthorized(userId))
	{
		userDB.SendErrorMessageToUser(":You may not reregister", userId,
									  M_ERR_ALREADYREGISTRED, userId);
		return ;
	}

	std::string userName = message.GetParameters().at(0);
	std::string hostName = message.GetParameters().at(1);
	std::string hostAddress = message.GetParameters().at(2);
	userDB.SetUserName(userId, userName);
	userDB.SetHostName(userId, hostName);
	userDB.SetHostAddress(userId, hostAddress);

	const std::string&	nickname = UserDB::GetInstance().GetNickName(userId);

	if (userDB.IsUserAuthorized(userId))
	{
		Server::GetInstance().AuthorizeUser(userId);
		userDB.SendErrorMessageToUser(":Welcome to the "
									+ userDB.GetHostAddress(userId)
									+ " Network, " + nickname, userId,
									  M_RPL_WELCOME, userId);
	}
}
