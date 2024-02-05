#include <set>
#include "Server.hpp"
#include "UserDB.hpp"
#include "ChannelDB.hpp"
#include "Message.hpp"
#include <string>

enum {

    ERR_NONICKNAMEGIVEN = 431, //no param
    ERR_ERRONEUSNICKNAME = 432, //서버 금지 닉넴 + 형식 오류 (leading '#', '&', ':' || contains ' ')
    ERR_NICKNAMEINUSE = 433, // UserDB::hasNickname(nick)
    ERR_NICKCOLLISION = 436 // 다른 서버에 있는 닉넴 (안해도 됨)

};

//TODO enum header
//TODO UserDB::contains(nick)
//TODO server&, userDB&, ChannelDB& Command에 넣으면 안될까?
//TODO UserDB::hasNickname(nick)
//TODO Command::mCommand 왜 필요?
//TODO Server::SendErrCodeToClient(int socketFd, int errCode, std::string errMsg) 함수 있으면 편할듯
//TODO Server::SendMessageToClient(int socketFd, string data) 안에서 char*로 바꾸고 길이도 재주고 가능?

namespace
{
	bool isForbiddenName(std::string nickname)
	{
		char firstChar = nickname.at(0);

		if (firstChar == '#' || firstChar == '&' || firstChar == ':')
		{
			return true;
		}

		return false;
	}

}

void	HookFunctionNick(const Message& message)
{
	Server&		server = Server::GetInstance();
	UserDB&		userDB = UserDB::GetInstance();
	int			socketFd = message.GetUserId();

	if (message.GetParameters().size() == 0)
	{
		//server.SendErrCodeToClient(socketFd, ERR_NONICKNAMEGIVEN);
		return;
	}

	std::string nickname = message.GetParameters().at(0);

	if (isForbiddenName(nickname))
	{
		//server.SendErrCodeToClient(socketFd, ERR_ERRONEUSNICKNAME);
		return;
	}
	/*
	if (userDB.hasNickname(nickname))
	{
		server.SendErrCodeToClient(socketFd, ERR_NICKNAMEINUSE);
		return;
	}
	*/
	int userId = userDB.GetUserIdBySocketId(socketFd);
	userDB.SetNickName(userId, nickname);
}
