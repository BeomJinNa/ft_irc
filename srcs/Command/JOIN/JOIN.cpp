/*
1. 클라이언트에서 보내준 채널 이름이 이미 존재하는지 확인한다.
	> 없을시 채널을 생성하고 첫 클라이언트를 채널 운영자로 만들고 채널에 추가한다.
	> 있을시 클라이언트를 채널에 추가한다.
2. 채널에 클라이언트를 추가한다.
3. 채널에 클라이언트가 추가되었음을 알린다.
그리고 모두 추가가 끝났음을 알린다. (366)
4. 채널에 있는 클라이언트들에게 새로운 클라이언트가 추가되었음을 알린다.
5. 만약 채널에서 새로운 업데이트(MODE, KICK, PART, QUIT, PRIVMSG/NOTICE.)가 있을시 채널에 있는 클라이언트에게 모두 알려야한다.


채널에 초대가능한 클라이언트 목록인 isuserinvited 를 만들어져있으니 만약 channel플래그에 +i가 있을시 초대된 클라이언트만 채널에 들어올수있게 구현하기.
*/
#include "ChannelDB.hpp"
#include "Message.hpp"
#include "UserDB.hpp"
#include <iostream>
#include "ErrorCodes.hpp"
#include "Enum.hpp"
#include "ChannelMode.hpp"
namespace{
	// std::string addAllMessage(std::string command, std::string channelName, int GetUserId)
	// {
	// 	UserDB userDB;
	// 	std::string username = userDB.GetUserName(GetUserId);

	// 	if(command == "MEJOIN")
	// 		return "JOIN " + username;
	// 	else if(command == "SOMEONEJOIN")
	// 		return username + " has joined";
	// 	else
	// 		return "";
	// }
	bool CheckParse(std::string channelName)
	{
		if(channelName[0] == '#' && channelName.length() > 1 && channelName.length() <= 200)
			return true;
		else
			return false;
	}
	bool CheckInviteOnly(int channelid, int userId)
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
		// TODO: 0x0001000 = +k 라고 가정하고 구현
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
	bool compareChannelKey(int channelid, std::string& parsedKeys)
	{
		ChannelDB&	channelDB = ChannelDB::GetInstance();
		if(channelDB.GetChannelPassword(channelid) == parsedKeys)
			return true;
		else
			return false;
	}

	// join #1,#2,#3 key1,key2,key3
	// parameters: #1,#2,#3 or key1,key2,key3
	// ,를 기준으로 파씽한 이후 vector에 담아서 반환한다.
	//동적할당은 하지 않는다.
	std::vector<std::string> &parseParameters(const std::string& parameters)
	{
		std::vector<std::string> result;
		std::string::size_type pos = 0, prev = 0;
		while((pos = parameters.find(',', prev)) != std::string::npos)
		{
		    result.push_back(parameters.substr(prev, pos - prev));
		    prev = pos + 1;
		}
		result.push_back(parameters.substr(prev));
		return result;
	}
	// int errorcheck(const Message& message)
	// {
	// 	if(message.GetParameters().size() < 1)
	// 	{
	// 		userDB.SendErrorMessageToUser("#1.2 :Invalid channel name", userId, M_ERR_BADCHANMASK,userId);
	// 		return 1;
	// 	}
	// 	return 0;
	// }
}

void	HookFunctionJoin(const Message& message)
{
	ChannelDB&					channelDB = ChannelDB::GetInstance();
	UserDB&						userDB = UserDB::GetInstance();
	int							userId = message.GetUserId();
	std::vector<std::string>	parsedChannelNames;
	std::vector<std::string>	parsedKeys;

	// errorcheck(message);

	if (message.GetParameters().size() < 1)
	{
		userDB.SendErrorMessageToUser(":Invalid channel name", userId, M_ERR_NEEDMOREPARAMS, userId);
		return;
	}
	//파씽 채널명이랑 키파씽하기.
	parsedChannelNames = parseParameters(message.GetParameters().at(0));
	parsedKeys = parseParameters(message.GetParameters().at(1));
	for(int i = 0; i < parsedChannelNames.size(); i++)
	{
		const std::string&	channelName = parsedChannelNames[i];
		int					channelId = channelDB.GetChannelIdByName(channelName);

		if (isMaxUserLimitOn(channelId))
		{
			if(channelDB.GetCurrentUsersInChannel(channelId) >= channelDB.GetMaxUsersInChannel(channelId))
			{
				userDB.SendErrorMessageToUser(channelName + " :Cannot join channel (+l)", userId, ERR_CHANNELISFULL, userId);
				continue;
			}
		}
		if(!CheckParse(channelName))
		{//TODO: 채널명/메세지/유저이름/에러코드를 보내야한다.
		//127.000.000.001.06667-127.000.000.001.35066: :irc.local 476 itsmemario #. :Invalid channel name
			userDB.SendErrorMessageToUser(channelName + " :Bad Channel Mask", userId, M_ERR_BADCHANMASK,userId);
			continue;
		}
		if (channelId == -1) //채널이 존재하지 않을때 (-1) 생성하고 클라이언트를 입장시킨다.
		{
			channelId = channelDB.CreateChannel(channelName);
			channelDB.AddOperatorIntoChannel(channelId, message.GetUserId());
		}
		else
		{
			//invite-only 모드인지 확인후 인바이트 된 클라이언트인지 확인하고 아니면 에러메세지를 보내고 continue
			if(CheckInviteOnly(channelId, userId))
				if (channelDB.IsUserInvited(channelId, userId) == false)
				{
					//127.000.000.001.06667-127.000.000.001.58710: :irc.local 473 mikim3 #1 :Cannot join channel (invite only)
					userDB.SendErrorMessageToUser(channelName + " :Cannot join channel (+i)", userId, ERR_INVITEONLYCHAN,userId);
					continue;
				}
			//key가 있는 채널인지 확인후, 키가 맞는지 확인하고 아니면 에러메세지를 보내고 continue
			if(doesChannelRequirePassword(channelId))
				if(compareChannelKey(channelId, parsedKeys[i]) == false) //i
				{
					//127.000.000.001.06667-127.000.000.001.54044: :irc.local 475 user1 #1 :Cannot join channel (incorrect channel key)
					userDB.SendErrorMessageToUser(channelName + " :Cannot join channel (+k)", userId, ERR_BADCHANNELKEY, userId);
					continue;
				}
		}
		channelDB.AddUserIntoChannel(message.GetUserId(), channelId);
		channelDB.SendMessageToChannel(userDB.GetUserName(userId) + " has joined", channelId);

		const ChannelDB::UserList& userList = channelDB.GetUserListInChannel(channelId);

		std::string userNames = "";
		for (ChannelDB::UserList::const_iterator it = userList.begin(); it != userList.end(); ++it)
		{
			userNames += userDB.GetUserName(*it) + " ";
		}
		//  "<client> = <channel> :[prefix]<nick>{ [prefix]<nick>}"
		channelDB.SendFormattedMessageToChannel("= " + channelName + " :" + userNames, channelId); //TODO: RPL_NAMREPLY (353) 보내기? 366해야함.
		channelDB.SendFormattedMessageToChannel("366 " + channelName + " :End of /NAMES list", channelId);
	}
}

/*

	알게된점 : 서버채널 접속할때 서버*
	ERR_TOOMANYCHANNELS (405) 합의결과 채널에 접속할수있는 최대수를 넘었을때의 에러처리는 지금 필요없는것같아서 하지 않았음.
	ERR_BADCHANMASK (476) 채널명을 올바르게 표시하진 않았으나 비슷한 채널명이 있을경우 뱉는 에러 ex) #Hello가 올바른 채널이름인데 #hello라는 유사한 채널명을 입력했을시 뱉는 에러 썼네^^
	ERR_BANNEDFROMCHAN (474) 채널에 차단당한 클라이언트가 채널에 접속하려고 할때 뱉는 에러 할필요없음
	TODO: topic해야하는데 아직 안만들어져서 못했음. (RPL_TOPICWHOTIME (333)와 RPL_TOPIC (332)는 만들면 하기로함.)

	TODO: 파씽자잘한것만 고치면 끝?

*/
