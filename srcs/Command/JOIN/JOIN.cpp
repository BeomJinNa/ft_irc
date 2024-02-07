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
}

void	HookFunctionJoin(const Message& message)
{
	try{
		ChannelDB&	channelDB = ChannelDB::GetInstance();
		for(int i = 0; i < message.GetParameters().size(); i++)
		{
			int newchannelID;
			int channelid = channelDB.GetChannelIdByName(message.GetParameters().at(i));
			if (channelid == -1)
			{
				newchannelID = channelDB.CreateChannel(message.GetParameters().at(i));
				channelDB.AddUserIntoChannel(message.GetUserId(), newchannelID);
				channelDB.SetChannelOperator(message.GetUserId(), message.GetParameters().at(i));
				channelDB.SendMessageToChannel(addAllMessage("MEJOIN",message.GetParameters().at(i), message.GetUserId()), newchannelID);
			}
			else
			{
				channelDB.AddUserIntoChannel(message.GetUserId(), newchannelID);
				channelDB.SendMessageToChannel(addAllMessage("MEJOIN",message.GetParameters().at(i), message.GetUserId()), newchannelID);
			}

		}
	}
	catch(const std::string *e){
		std::cout << "Error : " << e << '\n';
	}

}
/*알게된점 : 서버채널 접속할때 서버*/
