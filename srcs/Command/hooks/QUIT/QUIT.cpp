#include "UserDB.hpp"
#include "ChannelDB.hpp"
#include "Message.hpp"

void	HookFunctionQuit(const Message& message)
{
	UserDB&				userDB = UserDB::GetInstance();
	ChannelDB&			channelDB = ChannelDB::GetInstance();
	int					userId = message.GetUserId();
	const std::string&	trailing = message.GetTrailing();

	std::string	quitMessage;
	if (trailing.empty())
	{
		quitMessage = userDB.GetNickName(userId) + " has quit.";
	}
	else
	{
		quitMessage = userDB.GetNickName(userId) + " :" + trailing;
	}

	//TODO
	//텍스트를 IRC 규격에 맞게 수정해야 함
	UserDB::ChannelList	list = userDB.GetJoinnedChannelList(userId);
	for (UserDB::ChannelList::iterator it = list.begin(); it != list.end(); ++it)
	{
		std::string	sendMessage = "#" + channelDB.GetChannelName(*it)
								+ " :" + quitMessage;
		channelDB.SendMessageToChannel(sendMessage, *it);
	}
	userDB.DisconnectUser(userId);
}
