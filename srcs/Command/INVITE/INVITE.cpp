#include <set>
#include <string>
#include <vector>
#include <sstream>
#include <utility>
#include "Server.hpp"
#include "UserDB.hpp"
#include "ChannelDB.hpp"
#include "ChannelMode.hpp"
#include "Message.hpp"
#include "ErrorCodes.hpp"
#include "ReplyCodes.hpp"

/*

	INVITE <nickname> <channel>

    RPL_INVITING (341)			"<client> <nick> <channel>"
    ERR_NEEDMOREPARAMS (461)	"<client> <command> :Not enough parameters"
    ERR_NOSUCHCHANNEL (403)		"<client> <channel> :No such channel"
    ERR_NOTONCHANNEL (442)		"<client> <channel> :You're not on that channel"
    ERR_CHANOPRIVSNEEDED (482)	"<client> <channel> :You're not channel operator"
    ERR_USERONCHANNEL (443)		"<client> <nick> <channel> :is already on channel"


	/invite #new2 yujin

	127.000.000.001.53540-127.000.000.001.06667: INVITE yujin #new2
	127.000.000.001.06667-127.000.000.001.53540: :irc.local 341 hcho2_ yujin :#new2
	127.000.000.001.06667-127.000.000.001.40210: :hcho2_!codespace@127.0.0.1 INVITE yujin :#new2

	<server_hostname> <RPL_code> <sender_nickname>

*/

void	HookFunctionInvite(const Message& message)
{
	ChannelDB&			channelDB = ChannelDB::GetInstance();
	UserDB&				userDB = UserDB::GetInstance();
	int					userId = message.GetUserId();

	//parameter check
	if (message.GetParameters().size() < 2)
	{
		userDB.SendErrorMessageToUser("INVITE :Not enough parameters", userId, M_ERR_NEEDMOREPARAMS, userId);
		return ;
	}

	const std::string&	inviteeNickname = message.GetParameters().at(0);
	int					inviteeId = userDB.GetUserIdByNickName(inviteeNickname);
	const std::string&	channelName = message.GetParameters().at(1);
	int					channelId = channelDB.GetChannelIdByName(channelName);

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
	//check if channel is invite-only & sender is the operator of the channel
	if ((channelDB.GetChannelFlag(channelId) && M_FLAG_CHANNEL_INVITE_ONLY) && channelDB.IsUserOperator(channelId, userId))
	{
		userDB.SendErrorMessageToUser(channelName + ":You're not channel operator", userId, M_ERR_CHANOPRIVSNEEDED, userId);
		return ;
	}
	//check if invited user is already on the channel
	if (!channelDB.IsUserInChannel(channelId, inviteeId))
	{
		userDB.SendErrorMessageToUser(inviteeNickname + channelName + " :is already on channel", userId, M_ERR_NOTONCHANNEL, userId);
		return ;
	}

	channelDB.AddInvitedUserIntoChannel(channelId, inviteeId);

	// 127.000.000.001.53540-127.000.000.001.06667: INVITE yujin #new2
	// 127.000.000.001.06667-127.000.000.001.53540: :irc.local 341 hcho2_ yujin :#new2
	// 127.000.000.001.06667-127.000.000.001.40210: :hcho2_!codespace@127.0.0.1 INVITE yujin :#new2

	userDB.SendErrorMessageToUser(inviteeNickname + " " + channelName, userId, M_RPL_INVITING, userId);
	userDB.SendFormattedMessageToUser("INVITE " + inviteeNickname + " :" + channelName, userId, inviteeId);

}


/*

inspircd --runasroot --nofork
irssi -c 127.0.0.1 -p 6667 -n nickname
tcpflow -i lo port 6667 -c

*/
