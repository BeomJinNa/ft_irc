#include <arpa/inet.h>
#include <ctime>
#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "Command.hpp"
#include "Constant.hpp"
#include "FileContainer.hpp"
#include "FixedBufferArray.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "UserDB.hpp"

namespace
{
	int			xKevent(int mKq, const struct kevent *changelist,
						int nchanges, struct kevent *eventlist,
						int nevents, const struct timespec *timeout);
	Server*		TouchInstanceData(Server* address);
	int			getServerSocket(void);
	void		setSocketAddress(struct sockaddr_in& address, int port);
	void		bindServerSocket(int fd, struct sockaddr_in& address);
	void		listenServerSocket(int fd);
	int			getKqueue(void);
	void		setKqueue(struct kevent& kevent, int fdServer, int fdKqueue);
	uint16_t	getSocketPort(int socketFd);
	void		setConnectionTimeout(int clientFd, int kq, int timeoutSeconds);
	void		cancelConnectionTimeout(int clientFd, int kq);
}

Server::Server(int port)
{
	FileContainer	FC;
	mServerFd = getServerSocket();
	FC.AddFd(mServerFd);

	struct sockaddr_in	address;
	setSocketAddress(address, port);
	bindServerSocket(mServerFd, address);
	listenServerSocket(mServerFd);

	mKq = getKqueue();
	FC.AddFd(mKq);

	struct kevent	ev;
	setKqueue(ev, mServerFd, mKq);

	mHostAddress = "localhost";
	mHostPort = getSocketPort(mServerFd);

	std::ostringstream	oss;
	oss << mHostPort;
	mHostPortString = oss.str();

	TouchInstanceData(this);
	FC.SetSuccess();
}

Server::~Server(void)
{
	if (mServerFd != -1)
	{
		close(mServerFd);
	}
	if (mKq != -1)
	{
		close(mKq);
	}
	Server::CloseAllClientConnection();
}

void	Server::RunServer(void)
{
	while (true)
	{
		waitEvent();
	}
}

void	Server::SendMessageToClient(int clientFd, const char* data,
									size_t dataLength)
{
	mWriteBuffers[clientFd].append(data, dataLength);

	struct kevent	ev;
	EV_SET(&ev, clientFd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
	mWriteEvents.push_back(ev);
}

void	Server::CloseClientConnection(int clientFd)
{
	if (close(clientFd) == 0)
	{
		mClientFds.erase(clientFd);
		mReadBuffers.erase(clientFd);
		mReadSocketBuffers.erase(clientFd);
		mWriteBuffers.erase(clientFd);
	}
}

void	Server::CloseAllClientConnection(void)
{
	for (std::set<int>::iterator it = mClientFds.begin();
		 it != mClientFds.end(); ++it)
	{
		Server::CloseClientConnection(*it);
	}
}

void		Server::SetServerPassword(const std::string& password) { mPassword = password; }
std::string	Server::GetServerPassword(void) { return (mPassword); }
uint16_t	Server::GetHostPortNumber(void) const { return (mHostPort); }
std::string	Server::GetHostAddress(void) const { return (mHostAddress); }
std::string	Server::GetHostPort(void) const { return (mHostPortString); }

Server& Server::GetInstance(void)
{
	Server*	output = TouchInstanceData(NULL);

	if (output == NULL)
		throw std::runtime_error("Server Not Found");
	return (*output);
}

void	Server::waitEvent(void)
{
	const int		MAX_EVENTS = M_SERVER_KQUEUE_EVENT_BUFFER_SIZE;
	struct kevent	events[MAX_EVENTS];
	int				nev = xKevent(mKq, &mWriteEvents[0], mWriteEvents.size(),
								  events, MAX_EVENTS, NULL);
	UserDB&			userDB = UserDB::GetInstance();

	mWriteEvents.clear();
	for (int i = 0; i < nev; i++)
	{
		if (events[i].flags & (EV_EOF | EV_ERROR))
		{
			if (events[i].ident == static_cast<uintptr_t>(mServerFd))
			{
				userDB.RemoveUserData(userDB.GetUserIdBySocketId(events[i].ident));
				CloseClientConnection(events[i].ident);
			}
		}
		else if (events[i].filter == EVFILT_TIMER)
		{
			userDB.RemoveUserData(userDB.GetUserIdBySocketId(events[i].ident));
			CloseClientConnection(events[i].ident);
		}
		else if (events[i].filter == EVFILT_READ)
		{
			if (events[i].ident == static_cast<uintptr_t>(mServerFd))
			{
				acceptConnection();
			}
			else
			{
				handleRead(events[i].ident);
			}
		}
		else if (events[i].filter == EVFILT_WRITE)
		{
			handleWrite(events[i].ident);
		}
	}
}

void Server::acceptConnection(void)
{
	struct sockaddr_in	clientAddr;
	socklen_t			clientLen = sizeof(clientAddr);
	int					clientFd = accept(mServerFd, (struct sockaddr*)&clientAddr,
										  &clientLen);

	if (clientFd >= 0)
	{
		fcntl(clientFd, F_SETFL, O_NONBLOCK);

		struct kevent	ev;
		EV_SET(&ev, clientFd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		xKevent(mKq, &ev, 1, NULL, 0, NULL);
		mClientFds.insert(clientFd);
		setConnectionTimeout(clientFd, mKq, M_SERVER_CONNECTION_TIMEOUT_SECONDS);

		bool isConnectionAvailable = UserDB::GetInstance().ConnectUser(clientFd);
		if (isConnectionAvailable == false)
		{
			CloseClientConnection(clientFd);
		}
	}
}

void Server::handleRead(int clientFd)
{
	static const size_t		bufferSize = M_SERVER_READ_BUFFER_SIZE;
	ssize_t					bytes_read = read(clientFd,
											  mReadSocketBuffers[clientFd].buffer,
											  bufferSize - 1);
	UserDB&					userDB = UserDB::GetInstance();

	if (bytes_read > 0)
	{
		mReadSocketBuffers[clientFd].buffer[bytes_read] = '\0';
		mReadBuffers[clientFd].append(mReadSocketBuffers[clientFd].buffer);

#ifdef LOG_ON
		std::string	dubugMessage(mReadSocketBuffers[clientFd].buffer);
		std::size_t	pos = dubugMessage.find("\r\n");
		if (pos == std::string::npos)
		{
			std::cout << "<socket:recv> " << dubugMessage << std::endl;
		}
		else
		{
			dubugMessage = dubugMessage.substr(0, pos);
			std::cout << "<socket:recv> " << dubugMessage << std::endl;
		}
#endif

		size_t	end_of_msg = mReadBuffers[clientFd].find("\r\n");
		while (end_of_msg != std::string::npos)
		{
			if (end_of_msg > M_IRC_MAX_MESSAGE_LENGTH)
			{
				CloseClientConnection(clientFd);
				return ;
			}
			std::string message = mReadBuffers[clientFd].substr(0, end_of_msg);
#ifdef LOG_ON
			std::cout << "\033[33m<recv> " << message << "\033[0m" << std::endl;
#endif
			mReadBuffers[clientFd].erase(0, end_of_msg + 2);
			executeHooks(userDB.GetUserIdBySocketId(clientFd), message);
			end_of_msg = mReadBuffers[clientFd].find("\r\n");
		}
	}
	else
	{
		userDB.RemoveUserData(userDB.GetUserIdBySocketId(clientFd));
		CloseClientConnection(clientFd);
	}
}

void	Server::executeHooks(int userId, std::string message)
{
	Message	parser;

	bool	parsingSuccess = parser.ParseMessage(userId, message);

	if (parsingSuccess)
	{
		Command::ExecuteCommand(parser);
	}
}

void	Server::handleWrite(int clientFd)
{
	std::string&	buffer = mWriteBuffers[clientFd];

	if (!buffer.empty())
	{
		ssize_t	bytesSent = write(clientFd, buffer.c_str(), buffer.size());
		if (bytesSent > 0)
		{
			buffer.erase(0, bytesSent);
		}
		if (buffer.empty())
		{
			struct kevent	ev;
			EV_SET(&ev, clientFd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
			xKevent(mKq, &ev, 1, NULL, 0, NULL);
		}
	}
}

void	Server::AuthorizeUser(int userId)
{
	int	clientFd = UserDB::GetInstance().GetSocketIdByUserId(userId);

	cancelConnectionTimeout(clientFd, mKq);
}

namespace
{
	int	xKevent(int mKq, const struct kevent *changelist, int nchanges,
			struct kevent *eventlist, int nevents, const struct timespec *timeout)
	{
		int	retval = kevent(mKq, changelist, nchanges, eventlist, nevents, timeout);
		if (retval == -1)
		{
			throw std::runtime_error("kevent failed");
		}
		return (retval);
	}

	Server*	TouchInstanceData(Server* address)
	{
		static Server*	ServerGlobal = NULL;

		if (address != NULL)
		{ ServerGlobal = address;
			return (NULL);
		}

		return (ServerGlobal);
	}

	int	getServerSocket(void)
	{
		int	fd = socket(AF_INET, SOCK_STREAM, 0);
		if (fd == -1)
		{
			throw std::runtime_error("Socket creation failed");
		}
		fcntl(fd, F_SETFL, O_NONBLOCK);
		return (fd);
	}

	void	setSocketAddress(struct sockaddr_in& address, int port)
	{
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons(port);
	}

	void	bindServerSocket(int fd, struct sockaddr_in& address)
	{
		if (bind(fd, (struct sockaddr*)&address, sizeof(address)) < 0)
		{
			throw std::runtime_error("Bind failed");
		}
	}

	void	listenServerSocket(int fd)
	{
		if (listen(fd, M_SERVER_LISTEN_BACKLOG_QUEUE_SIZE) < 0)
		{
			throw std::runtime_error("Listen failed");
		}
	}

	int		getKqueue(void)
	{
		int fd = kqueue();
		if (fd == -1)
		{
			throw std::runtime_error("Failed to create mKqueue");
		}
		return (fd);
	}

	void	setKqueue(struct kevent& kevent, int fdServer, int fdKqueue)
	{
		EV_SET(&kevent, fdServer, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		if (xKevent(fdKqueue, &kevent, 1, NULL, 0, NULL) == -1)
		{
			throw std::runtime_error("Failed to add server socket event to mKqueue");
		}
	}

	uint16_t	getSocketPort(int socketFd)
	{
		struct sockaddr_in	address_input;
		socklen_t			address_input_len = sizeof(address_input);

		if (getsockname(socketFd, (struct sockaddr *)&address_input,
					&address_input_len) == -1)
		{
			throw std::runtime_error("getsockname failed");
		}
		return (ntohs(address_input.sin_port));
	}

	void	setConnectionTimeout(int clientFd, int kq, int timeoutSeconds)
	{
		struct kevent	ev;

		EV_SET(&ev, clientFd, EVFILT_TIMER, EV_ADD | EV_ENABLE,
			   0, timeoutSeconds * 1000, NULL);
		kevent(kq, &ev, 1, NULL, 0, NULL);
	}

	void	cancelConnectionTimeout(int clientFd, int kq)
	{
		struct kevent	ev;

		EV_SET(&ev, clientFd, EVFILT_TIMER, EV_DELETE, 0, 0, NULL);
		kevent(kq, &ev, 1, NULL, 0, NULL);
	}
}

void	Server::DoNothing(void) const {}
Server::Server(void) {}
Server::Server(const Server& source) { (void)source; }
Server&	Server::operator=(const Server& source)
{ if (this != &source) {} return (*this); }
