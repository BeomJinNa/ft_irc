#include <set>
#include "Server.hpp"
#include "UserDB.hpp"
#include "ChannelDB.hpp"
#include "Message.hpp"
#include <string>

enum {

    ERR_NONICKNAMEGIVEN = 431,
    ERR_ERRONEUSNICKNAME = 432,
    ERR_NICKNAMEINUSE = 433,
    ERR_NICKCOLLISION = 436

};

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
	Server&		server = Server::GetInstance();
	UserDB&		userDB = UserDB::GetInstance();
	int			userId = message.GetUserId();

	if (message.GetParameters().empty())
	{
		userDB.SendErrorMessageToUser(":Not enough parameters", userId, ERR_NONICKNAMEGIVEN);
		return ;
	}

	std::string nickname = message.GetParameters().at(0);

	if (!isValidName(nickname))
	{
		userDB.SendErrorMessageToUser(":Invalid nickname", userId, ERR_ERRONEUSNICKNAME);
		return ;
	}
	if (userDB.GetUserIdByNickName(nickname) != -1)
	{
		userDB.SendErrorMessageToUser(":Nickname already in use", userId, ERR_NICKNAMEINUSE);
		return ;
	}

	userDB.SetNickName(userId, nickname);
}
