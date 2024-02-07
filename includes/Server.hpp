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

class FixedBufferArray;

class Server
{
	public:
		Server(int port);
		~Server(void);

		void			DoNothing(void) const;
		void			RunServer(void);
		void			SendMessageToClient(int clientFd, const char* data,
											size_t dataLength);
		void			CloseClientConnection(int clientFd);
		void			CloseAllClientConnection(void);
		void			SetServerPassword(const std::string& password);
		std::string		GetServerPassword(void);
		std::string		GetHostAddress(void) const;
		uint16_t		GetHostPortNumber(void) const;
		std::string		GetHostPort(void) const;

		static Server&	GetInstance(void);

	private:
		Server(void);
		Server(const Server& source);
		Server&	operator=(const Server& source);

		void	waitEvent(void);
		void	acceptConnection(void);
		void	handleRead(int clientFd);
		void	executeHooks(int userId, std::string message);
		void	handleWrite(int clientFd);

		int								mServerFd;
		int								mKq;
		std::set<int>					mClientFds;
		std::map<int, FixedBufferArray>	mReadSocketBuffers;
		std::map<int, std::string>		mReadBuffers;
		std::map<int, std::string>		mWriteBuffers;
		std::vector<struct kevent>		mWriteEvents;

		std::string						mPassword;
		std::string						mHostAddress;
		uint16_t						mHostPort;
		std::string						mHostPortString;
};
#endif
