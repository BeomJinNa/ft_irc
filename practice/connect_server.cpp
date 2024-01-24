#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int main()
{
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
	{
		std::cerr << "Socket creation failed" << std::endl;
		return (1);
	}

	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8080);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) == -1)
	{
		std::cerr << "Bind failed" << std::endl;
		close(server_fd);
		return (1);
	}

	if (listen(server_fd, 10) == -1)
	{
		std::cerr << "Listen failed" << std::endl;
		close(server_fd);
		return (1);
	}

	std::cout << "Server is listening on port 8080" << std::endl;

	int client_fd = accept(server_fd, NULL, NULL);
	if (client_fd == -1)
	{
		std::cerr << "Accept failed" << std::endl;
		close(server_fd);
		return (1);
	}

	// 여기에서 클라이언트와의 통신을 처리합니다.
	// 예: read(client_fd, buffer, sizeof(buffer))

	close(client_fd);
	close(server_fd);
	return (0);
}
