#include <string>
#include <vector>
#include <sstream>
#include <utility>
#include "UserDB.hpp"
#include "ChannelDB.hpp"
#include "Message.hpp"
#include "ErrorCodes.hpp"

namespace
{
	typedef std::vector<std::pair<int, std::string> > UserList;

	size_t getUserList(std::string param, UserList& userList)
	{
		UserDB&				userDB = UserDB::GetInstance();
		std::istringstream	iss(param);
		std::string			userName;

		while (std::getline(iss, userName, ','))
		{
			int userId = userDB.GetUserIdByNickName(userName);
			userList.push_back(std::make_pair(userId, userName));
		}

		return userList.size();
	}
}

void	HookFunctionKick(const Message& message)
{
	ChannelDB&			channelDB = ChannelDB::GetInstance();
	UserDB&				userDB = UserDB::GetInstance();
	int					channelId = channelDB.
									GetChannelIdByName(message.GetParameters().at(0));
	const std::string&	channelName = channelDB.GetChannelName(channelId);
	int					userId = message.GetUserId();

	if (userDB.IsUserAuthorized(userId) == false)
	{
		userDB.SendErrorMessageToUser("You have not registered", userId,
									   M_ERR_NOTREGISTERED, userId);
		return ;
	}

	if (message.GetParameters().size() < 2)
	{
		userDB.SendErrorMessageToUser("KICK :Not enough parameters", userId,
									  M_ERR_NEEDMOREPARAMS, userId);
		return ;
	}
	if (channelId == -1)
	{
		userDB.SendErrorMessageToUser(channelName + " :No such channel", userId,
									  M_ERR_NOSUCHCHANNEL, userId);
		return ;
	}

	if (!channelDB.IsUserInChannel(channelId, userId))
	{
		userDB.SendErrorMessageToUser(channelName + " :You're not on that channel",
									  userId, M_ERR_NOTONCHANNEL, userId);
		return ;
	}
	if (!channelDB.IsUserOperator(channelId, userId))
	{
		userDB.SendErrorMessageToUser(channelName + " :You're not channel operator",
									  userId, M_ERR_CHANOPRIVSNEEDED, userId);
		return ;
	}

	UserList			kickUsers;
	const std::string& 	user = message.GetParameters().at(1);
	std::string 		comment = message.GetTrailing();
	if (comment == "")
		comment = ":";

	getUserList(user, kickUsers);

	UserList::iterator it;

	for (it = kickUsers.begin(); it != kickUsers.end(); it++)
	{
		std::string&	kickUserNick = it->second;
		int				kickUserId = it->first;

		if (kickUserId == -1 || !channelDB.IsUserInChannel(channelId, kickUserId))
		{
			userDB.SendErrorMessageToUser(kickUserNick + " " + channelName
										+ " :They aren't on that channel",
										  userId, M_ERR_USERNOTINCHANNEL, userId);
			continue ;
		}

		std::string sendMessage = "KICK " + channelName + " "
								+ kickUserNick + " " + comment;

		channelDB.AnnounceFormattedToChannel(sendMessage, channelId, userId);
		channelDB.RemoveUserFromChannel(channelId, kickUserId);
	}
}
