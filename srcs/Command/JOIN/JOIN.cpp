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
		else
			return false;
	}

	bool CheckInviteOnly(int channelid)
	{
		ChannelDB&	channelDB = ChannelDB::GetInstance();
		if(channelDB.GetChannelFlag(channelid) & M_FLAG_CHANNEL_INVITE_ONLY)
			return true;
		else
			return false;
	}

	bool doesChannelRequirePassword(int channelId)
	{
		ChannelDB&	channelDB = ChannelDB::GetInstance();
		if(channelDB.GetChannelFlag(channelId) & M_FLAG_CHANNEL_PASSWORD_CHECK_ON)
			return true;
		else
			return false;
	}

	bool isMaxUserLimitOn(int channelId)
	{
		ChannelDB&	channelDB = ChannelDB::GetInstance();
		if(channelDB.GetChannelFlag(channelId) & M_FLAG_CHANNEL_MAX_USER_LIMIT_ON)
			return true;
		else
			return false;
	}

	bool compareChannelKey(int channelid, std::string parsedKey)
	{
		ChannelDB&	channelDB = ChannelDB::GetInstance();
		if(channelDB.GetChannelPassword(channelid) == parsedKey)
			return true;
		else
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

	bool CheckInviteOnlyAndHandle(ChannelDB& channelDB, UserDB& userDB, int channelId,
								  int userId, const std::string& channelName)
	{
		if(CheckInviteOnly(channelId) == true
		&& channelDB.IsUserInvited(channelId, userId) == false)
		{
			userDB.SendErrorMessageToUser(channelName + " :Cannot join channel (+i)",
			userId, M_ERR_INVITEONLYCHAN,userId);
			return false;
		}
		return true;
	}

	// bool CheckChannelKeyAndHandle(int channelId, int userId, const std::string& channelName, int keyCount, int i, const std::vector<std::string>& parsedKeys)
	// {
	// 	if (doesChannelRequirePassword(channelId) && compareChannelKey(channelId, keyCount <= i ? "" : parsedKeys[i]) == false)
	// 	{
	// 		UserDB::GetInstance().SendErrorMessageToUser(channelName + " :Cannot join channel (+k)", userId, M_ERR_BADCHANNELKEY, userId);
	// 		return false;
	// 	}
	// 	return true;
	// }

	bool CheckUserLimitAndHandle(ChannelDB& channelDB, UserDB& userDB, int channelId,
								 int userId, const std::string& channelName)
	{
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

	std::string getBotName()

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
			if(CheckInviteOnlyAndHandle(channelDB, userDB, channelId, userId, channelName)
					== false)
				continue;
			if (doesChannelRequirePassword(channelId)
			 && compareChannelKey(channelId, keyCount <= i ? "" : parsedKeys[i]) == false)
			{
				UserDB::GetInstance().SendErrorMessageToUser(
						channelName + " :Cannot join channel (+k)",
						userId, M_ERR_BADCHANNELKEY, userId);
				continue;
			}
			if (CheckUserLimitAndHandle(channelDB, userDB, channelId, userId, channelName)
					== false)
				continue;
		}
		//add new user to a channel
		channelDB.AddUserIntoChannel(channelId, userId);
		channelDB.SendFormattedMessageToChannel("JOIN :" + channelName, channelId, userId);

		//check topic
		std::string topic = channelDB.GetChannelTopic(channelId);
		if (topic != "")
			userDB.SendErrorMessageToUser(channelName + " :" + topic,
										  userId, M_RPL_TOPIC, userId);
		channelDB.SendFormattedMessageToChannel("TOPIC " + channelName + " " + topic,
												channelId);

		//send name reply
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
		userNames = oss.str();
		userDB.SendErrorMessageToUser("= " + channelName + " :" + userNames,
									  userId, M_RPL_NAMREPLY, userId);
		userDB.SendErrorMessageToUser(channelName + " :End of /NAMES list", userId,
									  M_RPL_ENDOFNAMES, userId);
		channelDB.SendMessageToChannel("PRIVMSG " + channelName + " :bot!bot@localhost :Hello filthy humans!",
									   channelId);

}
