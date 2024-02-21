#include <string>
#include "UserDB.hpp"
#include "Message.hpp"
#include "ErrorCodes.hpp"

void	HookFunctionPing(const Message& message)
{
	UserDB&			userDB = UserDB::GetInstance();
	int				userId = message.GetUserId();

	if (message.GetParameters().size() == 0)
	{
		userDB.SendErrorMessageToUser("PING :Not enough parameters",
									  userId, M_ERR_NEEDMOREPARAMS, userId);
		return ;
	}

	if (userDB.IsUserAuthorized(userId) == false)
	{
		userDB.SendErrorMessageToUser("You have not registered", userId,
									   M_ERR_NOTREGISTERED, userId);
		return ;
	}

	const std::string&	token = message.GetParameters().at(0);
	std::string	hostname
		= userDB.GetNickName(userId)
		+ "!" + userDB.GetUserName(userId)
		+ "@" + userDB.GetHostAddress(userId);

	std::string	sendingMessage
		= ":" + hostname
		+ " PONG " + hostname
		+ " " + token;
	userDB.SendMessageToUser(sendingMessage, userId);
}
