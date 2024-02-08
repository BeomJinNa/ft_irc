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
	std::string addAllMessage(std::string command, std::string channelName, int GetUserId)
	{
		UserDB userDB;
		std::string username = userDB.GetUserName(GetUserId);

		if(command == "MEJOIN")
			return "JOIN " + username;
		else if(command == "SOMEONEJOIN")
			return username + " has joined";
		else
			return "";
	}
	bool Checkparse(std::string channelName)
	{
		if(channelName[0] == '#' && channelName.length() > 1 && channelName.length() <= 200)
			return true;
		else
			return false;
	}
	bool CheckInviteOnly(int channelid, int userId)
	{
		ChannelDB&	channelDB = ChannelDB::GetInstance();
		// TODO: 0x000010 = +i 라고 가정하고 구현
		if(channelDB.GetChannelFlag(channelid) & M_FLAG_CHANNEL_INVITE_ONLY)
			return true;
		else
			return false;
	}
	bool isChannelNeedPassword(int channelid)
	{
		ChannelDB&	channelDB = ChannelDB::GetInstance();
		// TODO: 0x0001000 = +k 라고 가정하고 구현
		if(channelDB.GetChannelFlag(channelid) & M_FLAG_CHANNEL_PASSWORD_CHECK_ON)
			return true;
		else
			return false;
	}
	bool compareChannelKey(int channelid, std::string& parseKey)
	{
		ChannelDB&	channelDB = ChannelDB::GetInstance();
		if(channelDB.GetChannelPassword(channelid) == parseKey)
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
}

void	HookFunctionJoin(const Message& message)
{
	try{
		ChannelDB&	channelDB = ChannelDB::GetInstance();
		UserDB&		userDB = UserDB::GetInstance();
		int			userId = message.GetUserId();
		std::vector<std::string>	parseChannel;
		std::vector<std::string>	parseKey;

		//파씽 채널명이랑 키파씽하기.
		parseChannel = parseParameters(message.GetParameters().at(0));
		parseKey = parseParameters(message.GetParameters().at(1));
		for(int i = 0; i < parseChannel.size(); i++)
		{
			int newchannelID;
			int channelId = channelDB.GetChannelIdByName(parseChannel[i]);
			if(!Checkparse(message.GetParameters().at(i)))
			{//TODO: 채널명/메세지/유저이름/에러코드를 보내야한다.
				userDB.SendErrorMessageToUser("#1.2 :Invalid channel name", userId, M_ERR_BADCHANMASK);
				continue;
			}
			if (channelId == -1) //채널이 존재하지 않을때 (-1) 생성하고 클라이언트를 입장시킨다.
			{
				newchannelID = channelDB.CreateChannel(message.GetParameters().at(i));
				channelDB.AddOperatorIntoChannel(channelId, message.GetUserId());
			}
			else
			{
				//invite-only 모드인지 확인후 인바이트 된 클라이언트인지 확인하고 아니면 에러메세지를 보내고 continue
				if(CheckInviteOnly(channelId, userId))
					if (channelDB.IsUserInvited(channelId, userId) == false)
					{
						//127.000.000.001.06667-127.000.000.001.58710: :irc.local 473 mikim3 #1 :Cannot join channel (invite only)
						userDB.SendErrorMessageToUser("#1 :Cannot join channel (invite only)", userId, ERR_INVITEONLYCHAN);
						continue;
					}
				//key가 있는 채널인지 확인후, 키가 맞는지 확인하고 아니면 에러메세지를 보내고 continue
				if(isChannelNeedPassword(channelId))
					if(compareChannelKey(channelId, parseKey[i]) == false) //i
					{
						//127.000.000.001.06667-127.000.000.001.54044: :irc.local 475 user1 #1 :Cannot join channel (incorrect channel key)
						userDB.SendErrorMessageToUser("#1.2 :Invalid channel name", userId, ERR_BADCHANNELKEY);
						continue;
					}
			}
			channelDB.AddUserIntoChannel(message.GetUserId(), newchannelID);
			channelDB.SendMessageToChannel(addAllMessage("MEJOIN",message.GetParameters().at(i), message.GetUserId()), newchannelID);
		}
	}
	catch(const std::string *e){
		std::cout << "Error : " << e << '\n';
	}

}
/*알게된점 : 서버채널 접속할때 서버*/
