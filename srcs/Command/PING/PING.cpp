#include <set>
#include <string>
#include "Server.hpp"
#include "UserDB.hpp"
#include "ChannelDB.hpp"
#include "Message.hpp"
#include "ErrorCodes.hpp"
#include "ReplyCodes.hpp"

void	HookFunctionPing(const Message& message)
{
	Server&			server = Server::GetInstance();
	UserDB&			userDB = UserDB::GetInstance();
	int				userId = message.GetUserId();

	if (message.GetParameters().size() == 0)
	{
		userDB.SendErrorMessageToUser(":Not enough parameters", userId, M_ERR_NEEDMOREPARAMS, userId);
		return ;
	}

	const std::string&	token = message.GetParameters().at(0);
	std::string	hostname
		= userDB.GetNickName(userId)
		+ "!" + userDB.GetUserName(userId)
		+ "@" + server.GetHostAddress();

	std::string	sendingMessage
		= ":" + hostname
		+ " PONG " + hostname
		+ " " + token;
	userDB.SendMessageToUser(sendingMessage, userId);
}

// TODO: check message format again. hostname?
