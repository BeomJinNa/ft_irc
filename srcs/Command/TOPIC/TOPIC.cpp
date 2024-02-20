#include <ctime>
#include <set>
#include <sstream>
#include <string>
#include "Server.hpp"
#include "UserDB.hpp"
#include "ChannelDB.hpp"
#include "Message.hpp"
#include "ErrorCodes.hpp"
#include "ReplyCodes.hpp"

/*
namespace
{
	std::string TimeToString(time_t time)
	{
		const int size = 20;
		char buffer[size];
		strftime(buffer, size, "%Y-%m-%d-%H-%M-%S", gmtime(&time));
		return std::string(buffer);
	}
}
*/

void	HookFunctionTopic(const Message& message)
{
    ChannelDB&      channelDB = ChannelDB::GetInstance();
	UserDB&			userDB = UserDB::GetInstance();
	int				userId = message.GetUserId();

	if (message.GetParameters().size() == 0)
	{
		userDB.SendErrorMessageToUser("TOPIC :Not enough parameters", userId, M_ERR_NEEDMOREPARAMS, userId);
		return ;
	}

	std::string channelName = message.GetParameters().at(0);
	int	channelId = channelDB.GetChannelIdByName(channelName);

    if (channelId == -1)
    {
		userDB.SendErrorMessageToUser(channelName + " :No such channel", userId, M_ERR_NOSUCHCHANNEL, userId);
		return ;
    }

    if (!channelDB.IsUserInChannel(channelId, userId))
    {
		userDB.SendErrorMessageToUser(channelName + " :You're not on that channel", userId, M_ERR_NOTONCHANNEL, userId);
		return ;
    }

	if (!channelDB.IsUserOperator(channelId, userId))
	{
		userDB.SendErrorMessageToUser(channelName + " :You're not channel operator", userId, M_ERR_CHANOPRIVSNEEDED, userId);
		return ;
	}

	if (message.GetTrailing() == "")
	{
		std::string topic = channelDB.GetChannelTopic(channelId);

		if (topic == "")
		{
			userDB.SendErrorMessageToUser(channelName + " :No topic is set", userId, M_RPL_NOTOPIC, userId);
			return ;
		}

		userDB.SendErrorMessageToUser(channelName + " " + topic, userId, M_RPL_TOPIC, userId);

		std::string reply = channelName + " " + userDB.GetNickName(channelDB.GetChannelTopicSetUser(channelId)) + " :" + std::to_string(std::time(0));
		userDB.SendErrorMessageToUser(reply, userId, M_RPL_TOPICWHOTIME, userId);
		return;
	}

	channelDB.SetChannelTopic(channelId, message.GetTrailing(), userId);

	channelDB.SendFormattedMessageToChannel("TOPIC " + channelName + " " + channelDB.GetChannelTopic(channelId), channelId);
}
