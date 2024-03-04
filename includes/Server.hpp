#pragma once
#ifndef SERVER_HPP
# define SERVER_HPP

# include <fcntl.h>
# include <map>
# include <netinet/in.h>
# include <set>
# include <string>
# include <sys/event.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <unistd.h>
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
		uint16_t		GetHostPortNumber(void) const;
		std::string		GetHostAddress(void) const;
		std::string		GetHostPort(void) const;
		void			AuthorizeUser(int userId);

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
		void	setConnectionTimeout(int clientFd, int kq, int timeoutSeconds);
		void	cancelConnectionTimeout(int clientFd, int kq);

		int								mServerFd;
		int								mKq;
		std::set<int>					mClientFds;
		std::map<int, FixedBufferArray>	mReadSocketBuffers;
		std::map<int, std::string>		mReadBuffers;
		std::map<int, std::string>		mWriteBuffers;
		std::map<int, bool>				mKeventTimerExist;
		std::vector<struct kevent>		mWriteEvents;

		std::string						mPassword;
		uint16_t						mHostPort;
		std::string						mHostAddress;
		std::string						mHostPortString;
};
#endif
