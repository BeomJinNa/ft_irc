#include <string>
#include <vector>
#include "UserDB.hpp"
#include "ChannelDB.hpp"
#include "ChannelMode.hpp"
#include "Message.hpp"
#include "ErrorCodes.hpp"
#include "ReplyCodes.hpp"

namespace
{

}

void	HookFunctionInvite(const Message& message)
{
	ChannelDB&			channelDB = ChannelDB::GetInstance();
	UserDB&				userDB = UserDB::GetInstance();
	int					userId = message.GetUserId();

	if (message.GetParameters().size() < 2)
	{
		userDB.SendErrorMessageToUser("INVITE :Not enough parameters", userId, M_ERR_NEEDMOREPARAMS, userId);
		return ;
	}

	const std::string&	inviteeNickname = message.GetParameters().at(0);
	int					inviteeId = userDB.GetUserIdByNickName(inviteeNickname);
	const std::string&	channelName = message.GetParameters().at(1);
	int					channelId = channelDB.GetChannelIdByName(channelName);


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

	if ((channelDB.GetChannelFlag(channelId) & M_FLAG_CHANNEL_INVITE_ONLY) && !channelDB.IsUserOperator(channelId, userId))
	{
		userDB.SendErrorMessageToUser(channelName + " :You're not channel operator", userId, M_ERR_CHANOPRIVSNEEDED, userId);
		return ;
	}

	if (channelDB.IsUserInChannel(channelId, inviteeId))
	{
		userDB.SendErrorMessageToUser(inviteeNickname + channelName + " :is already on channel", userId, M_ERR_NOTONCHANNEL, userId);
		return ;
	}

	channelDB.AddInvitedUserIntoChannel(channelId, inviteeId);





	userDB.SendErrorMessageToUser(inviteeNickname + " " + channelName,
										userId, M_RPL_INVITING, userId);
	userDB.SendFormattedMessageToUser("INVITE " + inviteeNickname + " :" + channelName,
																		userId, inviteeId);
}
