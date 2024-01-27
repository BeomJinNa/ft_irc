#include <iostream>
#include "Message.hpp"

void handleJoinCommand(const Message& message)
{
	// message 객체에서 필요한 정보 추출
	std::string channel = message.GetParameters()[0];
	std::string user = message.GetPrefix();

	// 채널 입장 로직 처리
	std::cout << user << " joined " << channel << std::endl;
}
