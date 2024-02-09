#include <set>
#include <string>
#include <vector>
#include <sstream>
#include <utility>
#include "Server.hpp"
#include "UserDB.hpp"
#include "ChannelDB.hpp"
#include "Message.hpp"
#include "ErrorCodes.hpp"
#include "ReplyCodes.hpp"

/*

    ERR_NEEDMOREPARAMS (461) "<client> <command> :Not enough parameters"
    ERR_NOSUCHCHANNEL (403)	"<client> <channel> :No such channel"
    ERR_NOTONCHANNEL (442) "<client> <channel> :You're not on that channel"

*/

namespace
{
	typedef std::vector<std::pair<int, std::string>> ChannelList;

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

// PART <channel>{,<channel>} [<reason>]
// <reason> (if it exists) should be on each of these messages.

void	HookFunctionPart(const Message& message)
{
	ChannelDB&			channelDB = ChannelDB::GetInstance();
	UserDB&				userDB = UserDB::GetInstance();
	int					userId = message.GetUserId();

	// check if parameters are enough
	if (message.GetParameters().size() == 0)
	{
		userDB.SendErrorMessageToUser("PART :Not enough parameters", userId, M_ERR_NEEDMOREPARAMS, userId);
		return ;
	}

	ChannelList channelsToPart;
	// parse comma-separated channel names from the first parameter
	getChannelList(message.GetParameters().at(0), channelsToPart);

	ChannelList::iterator it;

	for (it = channelsToPart.begin(); it != channelsToPart.end(); it++)
	{
		int				channelId = it->first;
		std::string&	channelName = it->second;
		// check if channel exists
		if (channelId == -1)
		{
			userDB.SendErrorMessageToUser(channelName + " :No such channel", userId, M_ERR_NOSUCHCHANNEL, userId);
			return ;
		}
		//check if sender is on the channel
		if (!channelDB.IsUserInChannel(channelId, userId))
		{
			userDB.SendErrorMessageToUser(channelName + " :You're not on that channel", userId, M_ERR_NOTONCHANNEL, userId);
			return ;
		}
		// :dan-!d@localhost PART #test
		std::string sendMessage = "PART " + channelName;
		if (message.GetTrailing() != " ")
			sendMessage += " " + message.GetTrailing();

		channelDB.SendFormattedMessageToChannel(sendMessage, channelId);
		// remove user from the channel
		channelDB.RemoveUserIntoChannel(channelId, userId);
	}
}

//TODO: RemoveUserIntoChannel -> RemoveUserFromChannel
