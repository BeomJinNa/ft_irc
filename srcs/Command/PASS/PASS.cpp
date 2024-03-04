#include <string>
#include "ErrorCodes.hpp"
#include "Message.hpp"
#include "ReplyCodes.hpp"
#include "Server.hpp"
#include "UserDB.hpp"

void	HookFunctionPass(const Message& message)
{
	Server&	server = Server::GetInstance();
	UserDB&	userDB = UserDB::GetInstance();
	int		userId = message.GetUserId();

	if (message.GetParameters().size() == 0)
	{
		userDB.SendErrorMessageToUser("PASS :Not enough parameters", userId,
									  M_ERR_NEEDMOREPARAMS, userId);
		return ;
	}

	if (userDB.GetLoginStatus(userId))
	{
		userDB.SendErrorMessageToUser(":You may not reregister", userId,
									  M_ERR_ALREADYREGISTRED, userId);
		return ;
	}

	const std::string&	inputPassword = message.GetParameters().at(0);

	if (server.GetServerPassword() != inputPassword)
	{
		userDB.SendErrorMessageToUser(":Password incorrect", userId,
									  M_ERR_PASSWDMISMATCH, userId);
		return ;
	}

	userDB.SetLoginStatus(userId, true);
	if (userDB.IsUserAuthorized(userId))
	{
		server.AuthorizeUser(userId);
		userDB.SendErrorMessageToUser(":Welcome to the "
									+ userDB.GetHostAddress(userId)
									+ " Network, "
									+ UserDB::GetInstance().GetNickName(userId),
									  userId, M_RPL_WELCOME, userId);
	}
}
