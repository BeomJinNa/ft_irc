#pragma once
#ifndef SERVER_HPP
# define SERVER_HPP

# include <sys/types.h>
# include <sys/event.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>
# include <fcntl.h>
# include <string>
# include <map>
# include <set>
# include <vector>

class Server
{
	public:
		Server(int port);
		~Server(void);

		void	RunServer(void);
		void	SendMessageToClient(int fd, const char* data, size_t length);
		void	CloseClientConnection(int fd);
		void	CloseAllClientConnection(void);

	private:
		Server(void);
		Server(const Server& source);
		Server&	operator=(const Server& source);

		void	waitEvent(void);
		void	acceptConnection(void);
		void	handleRead(int fd);
		void	executeHooks(int clientFd, std::string message);
		void	handleWrite(int fd);

		int							mServerFd;
		int							mKq;
		std::map<int, char[1024]>	mReadSocketBuffers;
		std::map<int, std::string>	mReadBuffers;
		std::map<int, std::string>	mWriteBuffers;
		std::set<int>				mClientFds;
		std::vector<struct kevent>	mWriteEvents;
};
#endif
