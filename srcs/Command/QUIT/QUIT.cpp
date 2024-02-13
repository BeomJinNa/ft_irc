#include <set>
#include "Server.hpp"
#include "UserDB.hpp"
#include "ChannelDB.hpp"
#include "Message.hpp"

void	HookFunctionQuit(const Message& message)
{
	UserDB&				userDB = UserDB::GetInstance();
	ChannelDB&			channelDB = ChannelDB::GetInstance();
	int					userId = message.GetUserId();
	std::string			trailing = message.GetTrailing();

	if (userDB.IsUserAuthorized(userId) == false)
	{
		userDB.DisconnectUser(userId);
		return ;
	}

	if (trailing.empty())
	{
		trailing = ":" + userDB.GetNickName(userId) + " has quit.";
	}

	//:NickName!UserName@Host QUIT :trailing(message)
	std::string	quitMessage
		= ":" + userDB.GetNickName(userId)
		+ "!" + userDB.GetUserName(userId)
		+ "@" + userDB.GetHostAddress(userId)
		+ " QUIT " + trailing;

	std::set<int>	sendingUserList;

	UserDB::ChannelList	list = userDB.GetJoinnedChannelList(userId);
	for (UserDB::ChannelList::iterator it = list.begin(); it != list.end(); ++it)
	{
		std::vector<int>	usersInChannel = channelDB.GetUserListInChannel(*it);
		sendingUserList.insert(usersInChannel.begin(), usersInChannel.end());
	}
	sendingUserList.erase(userId);

	for (std::set<int>::iterator it = sendingUserList.begin();
		 it != sendingUserList.end(); ++it)
	{
		userDB.SendMessageToUser(quitMessage, *it);
	}
	userDB.DisconnectUser(userId);
}
