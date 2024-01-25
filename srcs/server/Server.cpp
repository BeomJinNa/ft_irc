#include <sys/socket.h>
#include <stdexcept>
#include <string>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctime>
#include <cerrno>
#include "Server.hpp"

namespace
{
	struct timespec	make_timespec(long seconds, long nanoseconds);
	int				xKevent(int mKq, const struct kevent *changelist, int nchanges,
							struct kevent *eventlist, int nevents, const struct timespec *timeout);
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

	if (listen(mServerFd, 10) < 0)
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

void	Server::SendMessageToClient(int fd, const char* data, size_t length)
{
	mWriteBuffers[fd].append(data, length);

	struct kevent	ev;
	EV_SET(&ev, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
	mWriteEvents.push_back(ev);
}

void	Server::CloseClientConnection(int fd)
{
	close(fd);
	mClientFds.erase(fd);
	mReadBuffers.erase(fd);
	mReadSocketBuffers.erase(fd);
	mWriteBuffers.erase(fd);
}

void	Server::CloseAllClientConnection(void)
{
	for (std::set<int>::iterator it = mClientFds.begin(); it != mClientFds.end(); ++it)
	{
		Server::CloseClientConnection(*it);
	}
}

void	Server::waitEvent(void)
{
	const int				MAX_EVENTS = 10;
	struct kevent			events[MAX_EVENTS];
	int						nev = xKevent(mKq, &mWriteEvents[0], mWriteEvents.size(),
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
	int					clientFd = accept(mServerFd, (struct sockaddr*)&clientAddr, &clientLen);

	if (clientFd >= 0)
	{
		fcntl(clientFd, F_SETFL, O_NONBLOCK);

		struct kevent	ev;
		EV_SET(&ev, clientFd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		xKevent(mKq, &ev, 1, NULL, 0, NULL);
		mClientFds.insert(clientFd);
	}
}

void Server::handleRead(int fd)
{
	static const size_t		bufferSize = 1024;
	ssize_t					bytes_read = read(fd, mReadSocketBuffers[fd], bufferSize - 1);

	mReadSocketBuffers[fd][bytes_read] = '\0';

	if (bytes_read > 0)
	{
		mReadBuffers[fd].append(mReadSocketBuffers[fd]);
		size_t	end_of_msg = mReadBuffers[fd].find("\r\n");
		if (end_of_msg == std::string::npos || mReadBuffers[fd].size() > 512)
		{
			CloseClientConnection(fd);
			return ;
		}
		std::string message = mReadBuffers[fd].substr(0, end_of_msg);
		mReadBuffers[fd].erase(0, end_of_msg + 2);
		executeHooks(fd, message);
	}
	else
	{
		CloseClientConnection(fd);
	}
}

/* TODO
 * 훅 구현하기
 */
void	Server::executeHooks(int clientFd, std::string message)
{
}

void	Server::handleWrite(int fd)
{
	std::string&	buffer = mWriteBuffers[fd];

	if (!buffer.empty())
	{
		ssize_t	bytesSent = write(fd, buffer.c_str(), buffer.size());
		if (bytesSent > 0)
		{
			buffer.erase(0, bytesSent);
		}
		if (buffer.empty())
		{
			struct kevent	ev;
			EV_SET(&ev, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
			xKevent(mKq, &ev, 1, NULL, 0, NULL);
		}
	}
}

namespace
{
	struct timespec	make_timespec(long seconds, long nanoseconds)
	{
		struct timespec	ts;

		ts.tv_sec = seconds;
		ts.tv_nsec = nanoseconds;

		return (ts);
	}

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
}

Server::Server(void) {}
Server::Server(const Server& source) { (void)source; }
Server&	Server::operator=(const Server& source)
{ if (this != &source) {} return (*this); }
