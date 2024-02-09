#include <set>
#include <string>
#include "Server.hpp"
#include "UserDB.hpp"
#include "ChannelDB.hpp"
#include "Message.hpp"
#include "ErrorCodes.hpp"
#include "ReplyCodes.hpp"

void	HookFunctionPass(const Message& message)
{
	Server&			server = Server::GetInstance();
	UserDB&			userDB = UserDB::GetInstance();
	int				userId = message.GetUserId();

	if (message.GetParameters().size() == 0)
	{
		userDB.SendErrorMessageToUser("PASS :Not enough parameters", userId, M_ERR_NEEDMOREPARAMS, userId);
		return ;
	}
	//
	if (userDB.GetLoginStatus(userId))
	{
		userDB.SendErrorMessageToUser(":You may not reregister", userId, M_ERR_ALREADYREGISTRED, userId);
		return ;
	}

	const std::string&	inputPassword = message.GetParameters().at(0);

	if (server.GetServerPassword() != inputPassword)
	{
		userDB.SendErrorMessageToUser(":Password incorrect", userId, M_ERR_PASSWDMISMATCH, userId);
		return ;
	}

	userDB.SetLoginStatus(userId, true);
}
