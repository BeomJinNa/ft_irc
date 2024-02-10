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

    ERR_NEEDMOREPARAMS (461)
    ERR_NOSUCHCHANNEL (403)
    ERR_CHANOPRIVSNEEDED (482)
    ERR_USERNOTINCHANNEL (441)
    ERR_NOTONCHANNEL (442)

*/

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

// KICK <channel> <user> *( "," <user> ) [<comment>]

void	HookFunctionKick(const Message& message)
{
	ChannelDB&			channelDB = ChannelDB::GetInstance();
	UserDB&				userDB = UserDB::GetInstance();
	int					channelId = channelDB.GetChannelIdByName(message.GetParameters().at(0));
	const std::string&	channelName = channelDB.GetChannelName(channelId);
	int					userId = message.GetUserId();

	// check if parameters are enough
	if (message.GetParameters().size() < 2)
	{
		userDB.SendErrorMessageToUser("KICK :Not enough parameters", userId, M_ERR_NEEDMOREPARAMS, userId);
		return ;
	}
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
	//check if sender is the operator of the channel
	if (!channelDB.IsUserOperator(channelId, userId))
	{
		userDB.SendErrorMessageToUser(channelName + " :You're not channel operator", userId, M_ERR_CHANOPRIVSNEEDED, userId);
		return ;
	}

	UserList				kickUsers;
	const std::string& 		user = message.GetParameters().at(1);
	std::string 			comment = message.GetTrailing();
	if (comment == "") // if trailing is not presented, send default message
		comment = ":";

	getUserList(user, kickUsers);

	UserList::iterator it;

	for (it = kickUsers.begin(); it != kickUsers.end(); it++)
	{
		std::string&	kickUserName = it->second;
		int				kickUserId = it->first;

		if (kickUserId == -1 || !channelDB.IsUserInChannel(channelId, kickUserId)) //No such user in channel
		{
			userDB.SendErrorMessageToUser(kickUserName + " " + channelName + " :They aren't on that channel", userId, M_ERR_NOTONCHANNEL, userId);
			continue ;
		}
		std::string sendMessage = "KICK " + kickUserName + " " + comment;
		channelDB.SendFormattedMessageToChannel(sendMessage, channelId);
		channelDB.RemoveUserIntoChannel(channelId, kickUserId);
	}
}
