#include <sys/socket.h>
#include <sys/event.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctime>
#include <cerrno>
#include <stdexcept>
#include <string>
#include "Server.hpp"
#include "UserDB.hpp"
#include "Message.hpp"
#include "Command.hpp"
#include "FixedBufferArray.hpp"
#include "Constant.hpp"

namespace
{
	int				xKevent(int mKq, const struct kevent *changelist,
							int nchanges, struct kevent *eventlist,
							int nevents, const struct timespec *timeout);
	Server*			TouchInstanceData(Server* address);
}

Server::Server(int port)
{
	mServerFd = socket(AF_INET, SOCK_STREAM, 0);
	if (mServerFd == -1)
	{
		throw std::runtime_error("Socket creation failed");
	}
	fcntl(mServerFd, F_SETFL, O_NONBLOCK);

	struct sockaddr_in	address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	if (bind(mServerFd, (struct sockaddr*)&address, sizeof(address)) < 0)
	{
		close(mServerFd);
		throw std::runtime_error("Bind failed");
	}

	if (listen(mServerFd, M_SERVER_LISTEN_BACKLOG_QUEUE_SIZE) < 0)
	{
		close(mServerFd);
		throw std::runtime_error("Listen failed");
	}

	mKq = kqueue();
	if (mKq == -1)
	{
		close(mServerFd);
		throw std::runtime_error("Failed to create mKqueue");
	}

	struct kevent	ev;
	EV_SET(&ev, mServerFd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	if (xKevent(mKq, &ev, 1, NULL, 0, NULL) == -1)
	{
		close(mServerFd);
		close(mKq);
		throw std::runtime_error("Failed to add server socket event to mKqueue");
	}
	TouchInstanceData(this);
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

void				Server::SetServerPassword(const std::string& password)
						{ mPassword = password; }
std::string&		Server::GetServerPassword(void) { return (mPassword); }
const std::string&	Server::GetServerPassword(void) const { return (mPassword); }

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

	mWriteEvents.clear();
	for (int i = 0; i < nev; i++)
	{
		if (events[i].filter == EVFILT_READ)
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
		if (events[i].filter == EVFILT_WRITE)
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

		bool isConnectionAvailable = UserDB::GetInstance().ConnectUser(clientFd);
		if (isConnectionAvailable == false)
		{
			close(clientFd);
		}
	}
}

void Server::handleRead(int clientFd)
{
	static const size_t		bufferSize = M_SERVER_READ_BUFFER_SIZE;
	ssize_t					bytes_read = read(clientFd,
											  mReadSocketBuffers[clientFd].buffer,
											  bufferSize - 1);

	if (bytes_read > 0)
	{
		mReadSocketBuffers[clientFd].buffer[bytes_read] = '\0';
		mReadBuffers[clientFd].append(mReadSocketBuffers[clientFd].buffer);

		size_t	end_of_msg = mReadBuffers[clientFd].find("\r\n");
		while (end_of_msg != std::string::npos)
		{
			if (end_of_msg > M_IRC_MAX_MESSAGE_LENGTH)
			{
				CloseClientConnection(clientFd);
				return ;
			}
			std::string message = mReadBuffers[clientFd].substr(0, end_of_msg);
			mReadBuffers[clientFd].erase(0, end_of_msg + 2);
			executeHooks(clientFd, message);
		}
	}
	else if (bytes_read == 0 || (bytes_read == -1 && errno == ECONNRESET))
	{
		UserDB::GetInstance().RemoveUserData(clientFd);
		CloseClientConnection(clientFd);
	}
}

void	Server::executeHooks(int clientFd, std::string message)
{
	Message	parser;

	bool	parsingSuccess = parser.ParseMessage(clientFd, message);

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

namespace
{
	int	xKevent(int mKq, const struct kevent *changelist, int nchanges,
			struct kevent *eventlist, int nevents, const struct timespec *timeout)
	{
		int	retval = kevent(mKq, changelist, nchanges, eventlist, nevents, timeout);
		if (retval == -1)
		{
			throw std::runtime_error("kevent failed: " + std::string(strerror(errno)));
		}
		return (retval);
	}

	Server*	TouchInstanceData(Server* address)
	{
		static Server*	ServerGlobal = NULL;

		if (address != NULL)
		{
			ServerGlobal = address;
			return (NULL);
		}

		return (ServerGlobal);
	}
}

void	Server::DoNothing(void) const {}
Server::Server(void) {}
Server::Server(const Server& source) { (void)source; }
Server&	Server::operator=(const Server& source)
{ if (this != &source) {} return (*this); }
