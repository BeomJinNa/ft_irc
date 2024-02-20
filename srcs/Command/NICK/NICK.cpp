#include <string>
#include "UserDB.hpp"
#include "Message.hpp"
#include "ErrorCodes.hpp"
#include "ReplyCodes.hpp"

namespace
{
	bool isValidName(std::string nickname)
	{
		char firstChar = nickname.at(0);

		if (firstChar == '#' || firstChar == '&' || firstChar == ':')
		{
			return false;
		}

		return true;
	}

}

void	HookFunctionNick(const Message& message)
{
	UserDB&		userDB = UserDB::GetInstance();
	int			userId = message.GetUserId();

	if (message.GetParameters().empty())
	{
		userDB.SendErrorMessageToUser(":No nickname given", userId, M_ERR_NONICKNAMEGIVEN, userId);
		return ;
	}

	std::string nickname = message.GetParameters().at(0);

	if (!isValidName(nickname))
	{
		userDB.SendErrorMessageToUser(nickname + " :Erroneus nickname", userId, M_ERR_ERRONEUSNICKNAME, userId);
		return ;
	}
	if (userDB.GetUserIdByNickName(nickname) != -1)
	{
		userDB.SendErrorMessageToUser(nickname + " :Nickname is already in use", userId, M_ERR_NICKNAMEINUSE, userId);
		return ;
	}

	userDB.SetNickName(userId, nickname);
	if (userDB.IsUserAuthorized(userId))
		userDB.SendErrorMessageToUser(":Welcome to the " + userDB.GetHostAddress(userId) + " Network, " + nickname, userId, M_RPL_WELCOME, userId);
}
