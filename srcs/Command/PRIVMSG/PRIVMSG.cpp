#include <set>
#include <string>
#include <algorithm>
#include "UserDB.hpp"
#include "ChannelDB.hpp"
#include "Message.hpp"
#include "ErrorCodes.hpp"

void split(const std::string& str, char delimiter, std::vector<std::string>& result)
{
	std::string::const_iterator		start = str.begin();
	std::string::const_iterator		end = str.begin();

	while (end != str.end())
	{
		end = std::find(start, str.end(), delimiter);
		result.push_back(std::string(start, end));
		start = end + 1;
	}
}

// prefix : sender
// <receiver>{,<receiver>} <text to be sent>
#include <iostream>
void	HookFunctionPrivmsg(const Message& message)
{
	UserDB&				userDB = UserDB::GetInstance();
	ChannelDB&		channelDB = ChannelDB::GetInstance();
	int						userId = message.GetUserId();

	if (message.GetParameters().size() == 0)
	{
		// no receiver
		userDB.SendErrorMessageToUser("Not enough parameters", userId, M_ERR_NEEDMOREPARAMS, userId);
		return;
	}
	std::vector<std::string> receivers;
	split(message.GetParameters()[0], ',', receivers);
	if (receivers.size() == 0)
	{
		// no receiver
		userDB.SendErrorMessageToUser("Not enough parameters", userId, M_ERR_NEEDMOREPARAMS, userId);
		return;
	}
	for (std::vector<std::string>::const_iterator it = receivers.begin(); it != receivers.end(); ++it)
	{
		if (it->empty())
		{
			// empty receiver
			continue;
		}
		const std::string		&receiver = *it;
		const std::string		&msg = message.GetTrailing();
		if (receiver[0] == '#')
		{
			// send to channel
			int channelId = channelDB.GetChannelIdByName(receiver.substr(1));
			if (channelId == -1)
			{
				// no such channel
				userDB.SendMessageToUser("No such channel", userId);
				continue;
			}

			// channelDB.SendMessageToChannel(msg, channelId);
			// send to user in channel except sender
			ChannelDB::UserList	list = channelDB.GetUserListInChannel(channelId);
			for (ChannelDB::UserList::const_iterator user_iter = list.cbegin(); user_iter != list.cend(); ++user_iter)
			{
				if (*user_iter == userId)
					continue;
				userDB.SendMessageToUser(msg, *user_iter);
			}
		}
		else
		{
			std::cout << "receiver: " << receiver << std::endl;
			// send to user
			int receiverId = userDB.GetUserIdByNickName(receiver);
			if (receiverId == -1)
			{
				// no such nick
				userDB.SendMessageToUser("No such nick", userId);
				continue;
			}
			userDB.SendMessageToUser(msg, receiverId);
		}
	}
}


/*
>> cases for PRIVMSG


127.000.000.001.46216 : A
127.000.000.001.34808 : B
127.000.000.001.03000 : Server

send message to unknown user)
127.000.000.001.46216-127.000.000.001.03000: PRIVMSG C :hello
127.000.000.001.03000-127.000.000.001.46216: :codespaces-b2e18e.local 401 A C :No such nick

send message to self)
127.000.000.001.34808-127.000.000.001.03000: PRIVMSG B :hello
127.000.000.001.03000-127.000.000.001.34808: :B!codespace@localhost PRIVMSG B :hello

send message to unknown channel)
127.000.000.001.34808-127.000.000.001.03000: PRIVMSG #ch2 :hello
127.000.000.001.03000-127.000.000.001.34808: :codespaces-b2e18e.local 403 B #ch2 :No such channel

<via nc>
not enough parameters)
- tests
	PRIVMSG
	PRIVMSG :hello
:codespaces-b2e18e.local 461 ncA USER :Not enough parameters.

1)
> scenario
A, B joined #ch1
A send msg to B,#ch1
> expected
Server send msg to B
Server send msg to B who joined #ch1
> result
Server send msg to B together.
> analysis
Server can send multiple message at once by combining the message.

A-Server: PRIVMSG B,#ch1 :test_msg2
Server-B: :A!codespace@localhost PRIVMSG B :test_msg2
:A!codespace@localhost PRIVMSG #ch1 :test_msg2

2)
> scenario
A, B joined #ch1
A send msg to A,B,#ch1
> expected
Server send msg to A, B and who joined #ch1
Server send msg to A
Server send msg to B
> result
Server send msg to B who joined #ch1. But not to A. // why?
Server send msg to A.
Server send msg to B.
> analysis
Server didn't send msg to A who joined #ch1.
why?
- hypothesis #1
	A send msg to channel so client can handle it. It's more efficient.
- test
	Send msg to channel
	let's check at 3), 4)


A-Server: PRIVMSG A,B,#ch1 :a_b_#ch1_test
Server-A: :A!codespace@localhost PRIVMSG A :a_b_#ch1_test
Server-B: :A!codespace@localhost PRIVMSG B :a_b_#ch1_test
:A!codespace@localhost PRIVMSG #ch1 :a_b_#ch1_test

3)
> scenario
A,B joined #ch1
A send msg to #ch1
> expected
Server send msg to B who joined #ch1. But not to A who joined channel and sent the msg.
> result
Server send msg to B who joined #ch1. But not to A.


A-Server: PRIVMSG #ch1 :hello
Server-B: :A!codespace@localhost PRIVMSG #ch1 :hello

4)
> scenario
A,B joined #ch1
A joined #ch2
A send msg to #ch1
> expected
Server send msg to B who joined #ch1. But not to A who joined channel and sent the msg.
Server will not send msg to A who joined #ch2. Because A sent the message.
> result
Server send msg to B who joined #ch1. But not to A.
Server didn't send msg to A who joined #ch2.


A-Server: PRIVMSG #ch1,#ch2 :test_ch1,ch2
Server-B: :A!codespace@localhost PRIVMSG #ch1 :test_ch1,ch2
*/