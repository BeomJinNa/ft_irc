#include "ChannelDB.hpp"
#include "Message.hpp"
#include "UserDB.hpp"
#include "ErrorCodes.hpp"
#include "ChannelMode.hpp"
#include "ReplyCodes.hpp"
#include <sstream>

namespace
{

	bool CheckParse(std::string channelName)
	{
		if(channelName[0] == '#' && channelName.length() > 1 && channelName.length() <= 200)
			return true;
		return false;
	}

	bool CheckInviteOnly(int channelid)
	{
		ChannelDB&	channelDB = ChannelDB::GetInstance();
		if(channelDB.GetChannelFlag(channelid) & M_FLAG_CHANNEL_INVITE_ONLY)
			return true;
		return false;
	}

	bool doesChannelRequirePassword(int channelId)
	{
		ChannelDB&	channelDB = ChannelDB::GetInstance();
		if(channelDB.GetChannelFlag(channelId) & M_FLAG_CHANNEL_PASSWORD_CHECK_ON)
			return true;
		return false;
	}

	bool isMaxUserLimitOn(int channelId)
	{
		ChannelDB&	channelDB = ChannelDB::GetInstance();
		if(channelDB.GetChannelFlag(channelId) & M_FLAG_CHANNEL_MAX_USER_LIMIT_ON)
			return true;
		return false;
	}

	bool compareChannelKey(int channelid, std::string parsedKey)
	{
		ChannelDB&	channelDB = ChannelDB::GetInstance();
		if(channelDB.GetChannelPassword(channelid) == parsedKey)
			return true;
		return false;
	}

	size_t parseParameters(const std::string& parameters, std::vector<std::string>& list)
	{
		std::string::size_type pos = 0, prev = 0;
		while((pos = parameters.find(',', prev)) != std::string::npos)
		{
			list.push_back(parameters.substr(prev, pos - prev));
			prev = pos + 1;
		}
		list.push_back(parameters.substr(prev));
		return list.size();
	}

	bool CheckInviteOnly(int channelId, int userId, const std::string& channelName)
	{
		ChannelDB&					channelDB = ChannelDB::GetInstance();
		UserDB&						userDB = UserDB::GetInstance();

		if(CheckInviteOnly(channelId) == true
		&& channelDB.IsUserInvited(channelId, userId) == false)
		{
			userDB.SendErrorMessageToUser(channelName + " :Cannot join channel (+i)",
			userId, M_ERR_INVITEONLYCHAN,userId);
			return false;
		}
		return true;
	}

	bool CheckChannelKey(int channelId, int userId, const std::string& channelName, const std::string& parsedKey)
	{
		if (doesChannelRequirePassword(channelId) && compareChannelKey(channelId, parsedKey) == false)
		{
			UserDB::GetInstance().SendErrorMessageToUser(channelName + " :Cannot join channel (+k)", userId, M_ERR_BADCHANNELKEY, userId);
			return false;
		}
		return true;
	}

	bool CheckUserLimit(int channelId, int userId, const std::string& channelName)
	{
		ChannelDB&					channelDB = ChannelDB::GetInstance();
		UserDB&						userDB = UserDB::GetInstance();

		if(isMaxUserLimitOn(channelId)
		&& channelDB.GetCurrentUsersInChannel(channelId)
		>= channelDB.GetMaxUsersInChannel(channelId))
		{
			userDB.SendErrorMessageToUser(channelName + " :Cannot join channel (+l)",
										  userId, M_ERR_CHANNELISFULL, userId);
			return false;
		}
		return true;
	}

	std::string getUserNames(int channelId)
	{
		ChannelDB&					channelDB = ChannelDB::GetInstance();
		UserDB&						userDB = UserDB::GetInstance();
		const ChannelDB::UserList& userList = channelDB.GetUserListInChannel(channelId);
		std::string userNames = "";
		std::ostringstream oss;

		for (ChannelDB::UserList::const_iterator it = userList.begin();
			 it != userList.end(); ++it)
		{
			if (channelDB.IsUserOperator(channelId, *it))
				oss << "@";
			oss << userDB.GetNickName(*it) << " ";
		}
		return oss.str();
	}
}

void	HookFunctionJoin(const Message& message)
{
	ChannelDB&					channelDB = ChannelDB::GetInstance();
	UserDB&						userDB = UserDB::GetInstance();
	int							userId = message.GetUserId();
	const std::string&			nickname = userDB.GetNickName(userId);
	std::vector<std::string>	parsedChannelNames;
	std::vector<std::string>	parsedKeys;
	size_t						keyCount = 0;

	if (message.GetParameters().size() < 1)
	{
		userDB.SendErrorMessageToUser(nickname + " :Not enough parameters",
									  userId, M_ERR_NEEDMOREPARAMS, userId);
		return;
	}
	parseParameters(message.GetParameters().at(0), parsedChannelNames);
	if (message.GetParameters().size() == 2)
		keyCount = parseParameters(message.GetParameters().at(1), parsedKeys);
	for(size_t i = 0; i < parsedChannelNames.size(); i++)
	{
		const std::string&	channelName = parsedChannelNames[i];

		if(!CheckParse(channelName))
		{
			userDB.SendErrorMessageToUser(channelName + " :Bad Channel Mask",
										  userId, M_ERR_BADCHANMASK,userId);
			continue;
		}

		int	channelId = channelDB.GetChannelIdByName(channelName);

		if (channelId == -1)
		{
			channelId = channelDB.CreateChannel(channelName);
			channelDB.AddOperatorIntoChannel(channelId, userId);
		}
		else
		{
			if(CheckInviteOnly(channelId, userId, channelName) == false)
				continue ;
			if (CheckChannelKey(channelId, userId, channelName, keyCount <= i ? "" : parsedKeys[i]) == false)
				continue ;
			if (CheckUserLimit(channelId, userId, channelName) == false)
				continue ;
		}

		channelDB.AddUserIntoChannel(channelId, userId);
		channelDB.AnnounceFormattedToChannel("JOIN :" + channelName, channelId, userId);


		std::string topic = channelDB.GetChannelTopic(channelId);
		if (topic != "")
			userDB.SendErrorMessageToUser(channelName + " :" + topic,
										  userId, M_RPL_TOPIC, userId);
		channelDB.AnnounceFormattedToChannel("TOPIC " + channelName + " " + topic,
												channelId);

		std::string userNames = getUserNames(channelId);
		userDB.SendErrorMessageToUser("= " + channelName + " :" + userNames,
									  userId, M_RPL_NAMREPLY, userId);
		userDB.SendErrorMessageToUser(channelName + " :End of /NAMES list", userId,
									  M_RPL_ENDOFNAMES, userId);
		channelDB.AnnounceToChannel(":welcome_bot!bot@localhost PRIVMSG " + channelName
									 + " :Welcome " + nickname + "!", channelId);
	}

}
