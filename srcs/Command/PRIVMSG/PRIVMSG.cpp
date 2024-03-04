#include <algorithm>
#include <string>
#include "ChannelDB.hpp"
#include "ErrorCodes.hpp"
#include "Message.hpp"
#include "UserDB.hpp"

namespace
{
	void split(const std::string& str, char delimiter, std::vector<std::string>& result)
	{
		std::string::const_iterator	start = str.begin();
		std::string::const_iterator	end = str.begin();

		while (end != str.end())
		{
			end = std::find(start, str.end(), delimiter);
			result.push_back(std::string(start, end));
			start = end + 1;
		}
	}
}

void	HookFunctionPrivmsg(const Message& message)
{
	UserDB&		userDB = UserDB::GetInstance();
	ChannelDB&	channelDB = ChannelDB::GetInstance();
	int			userId = message.GetUserId();

	if (message.GetParameters().size() == 0)
	{
		userDB.SendErrorMessageToUser(":Not enough parameters", userId,
									  M_ERR_NEEDMOREPARAMS, userId);
		return;
	}

	if (userDB.IsUserAuthorized(userId) == false)
	{
		userDB.SendErrorMessageToUser("You have not registered", userId,
									   M_ERR_NOTREGISTERED, userId);
		return;
	}

	std::vector<std::string> receivers;
	split(message.GetParameters()[0], ',', receivers);
	if (receivers.size() == 0)
	{
		userDB.SendErrorMessageToUser(":Not enough parameters", userId,
									  M_ERR_NEEDMOREPARAMS, userId);
		return;
	}
	for (std::vector<std::string>::const_iterator it = receivers.begin();
		 it != receivers.end(); ++it)
	{
		if (it->empty())
		{
			continue;
		}
		const std::string&	receiver = *it;
		const std::string&	msg = message.GetTrailing();
		if (receiver[0] == '#')
		{
			int channelId = channelDB.GetChannelIdByName(receiver);
			if (channelId == -1)
			{
				userDB.SendErrorMessageToUser(receiver + " :No such channel", userId,
											  M_ERR_NOSUCHCHANNEL, userId);
				continue;
			}

			ChannelDB::UserList	list = channelDB.GetUserListInChannel(channelId);
			for (ChannelDB::UserList::const_iterator user_iter = list.cbegin();
				 user_iter != list.cend(); ++user_iter)
			{
				if (*user_iter == userId)
					continue;
				userDB.SendFormattedMessageToUser("PRIVMSG " + receiver + " " + msg,
												  userId, *user_iter);
			}
		}
		else
		{
			int receiverId = userDB.GetUserIdByNickName(receiver);
			if (receiverId == -1)
			{
				userDB.SendErrorMessageToUser(receiver + " :No such nick/channel",
											  userId, M_ERR_NOSUCHNICK, userId);
				continue;
			}
			userDB.SendFormattedMessageToUser("PRIVMSG " + receiver + " " + msg,
											  userId, receiverId);
		}
	}
}
