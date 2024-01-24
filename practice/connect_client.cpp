#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int main()
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		std::cerr << "Socket creation failed" << std::endl;
		return (1);
	}

	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8080);
	inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

	if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) == -1)
	{
		std::cerr << "Connect failed" << std::endl;
		close(sock);
		return (1);
	}

	// 여기에서 서버에 데이터를 보내고 받습니다.
	// 예: send(sock, message, strlen(message), 0)

	close(sock);
	return (0);
}
