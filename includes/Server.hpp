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

enum ServerConstants
{
	M_READ_BUFFER_SIZE = 1024,
};

class Server
{
	public:
		Server(int port);
		~Server(void);

		void				RunServer(void);
		void				SendMessageToClient(int clientFd, const char* data, size_t length);
		void				CloseClientConnection(int clientFd);
		void				CloseAllClientConnection(void);
		void				SetServerPassword(const std::string& password);
		std::string&		GetServerPassword(void);
		const std::string&	GetServerPassword(void) const;

		static Server&	GetInstance(void);

	private:
		Server(void);
		Server(const Server& source);
		Server&	operator=(const Server& source);

		void	waitEvent(void);
		void	acceptConnection(void);
		void	handleRead(int clientFd);
		void	executeHooks(int clientFd, std::string message);
		void	handleWrite(int clientFd);

		int										mServerFd;
		int										mKq;
		std::set<int>							mClientFds;
		std::map<int, char[M_READ_BUFFER_SIZE]>	mReadSocketBuffers;
		std::map<int, std::string>				mReadBuffers;
		std::map<int, std::string>				mWriteBuffers;
		std::vector<struct kevent>				mWriteEvents;

		std::string								mPassword;
};
#endif
