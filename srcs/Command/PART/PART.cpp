#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "ChannelDB.hpp"
#include "ErrorCodes.hpp"
#include "Message.hpp"
#include "UserDB.hpp"

namespace
{
	typedef std::vector<std::pair<int, std::string> > ChannelList;

	size_t getChannelList(std::string param, ChannelList& channelList)
	{
		ChannelDB&			channelDB = ChannelDB::GetInstance();
		std::istringstream	iss(param);
		std::string			channelName;

		while (std::getline(iss, channelName, ','))
		{
			int channelId = channelDB.GetChannelIdByName(channelName);
			channelList.push_back(std::make_pair(channelId, channelName));
		}

		return channelList.size();
	}
}

void	HookFunctionPart(const Message& message)
{
	ChannelDB&	channelDB = ChannelDB::GetInstance();
	UserDB&		userDB = UserDB::GetInstance();
	int			userId = message.GetUserId();

	if (message.GetParameters().size() == 0)
	{
		userDB.SendErrorMessageToUser("PART :Not enough parameters",
									  userId, M_ERR_NEEDMOREPARAMS, userId);
		return ;
	}

	if (userDB.IsUserAuthorized(userId) == false)
	{
		userDB.SendErrorMessageToUser("You have not registered", userId,
									   M_ERR_NOTREGISTERED, userId);
		return ;
	}

	ChannelList channelsToPart;
	getChannelList(message.GetParameters().at(0), channelsToPart);

	ChannelList::iterator it;
	for (it = channelsToPart.begin(); it != channelsToPart.end(); it++)
	{
		int				channelId = it->first;
		std::string&	channelName = it->second;

		if (channelId == -1)
		{
			userDB.SendErrorMessageToUser(channelName + " :No such channel",
										  userId, M_ERR_NOSUCHCHANNEL, userId);
			return ;
		}
		if (!channelDB.IsUserInChannel(channelId, userId))
		{
			userDB.SendErrorMessageToUser(channelName + " :You're not on that channel",
										  userId, M_ERR_NOTONCHANNEL, userId);
			return ;
		}
		std::string sendMessage = "PART :" + channelName;

		channelDB.AnnounceFormattedToChannel(sendMessage, channelId, userId);
		channelDB.RemoveUserFromChannel(channelId, userId);
		if (channelDB.GetCurrentUsersInChannel(channelId) == 0)
		{
			channelDB.DeleteChannel(channelId);
		}
	}
}
