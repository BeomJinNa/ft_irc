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

/* TODO
 * 런타임 도중 kevent실패시 자원해제(소켓 해지)후 throw
 * run_server에서 kevent에 딜레이 설정 (시스템 콜 무한 호출 방지)
 * read, write 버퍼시스템 포함해서 구현하기 (get_next_line과 같은 형태)
 * 훅 구현하기
 */
namespace
{
	struct timespec	make_timespec(long seconds, long nanoseconds);
	int				xKevent(int kq, const struct kevent *changelist, int nchanges,
							struct kevent *eventlist, int nevents, const struct timespec *timeout);
}

Server::Server(int port)
{
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
	{
		throw std::runtime_error("Socket creation failed");
	}
	fcntl(server_fd, F_SETFL, O_NONBLOCK);

	struct sockaddr_in	address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
	{
		close(server_fd);
		throw std::runtime_error("Bind failed");
	}

	if (listen(server_fd, 10) < 0)
	{
		close(server_fd);
		throw std::runtime_error("Listen failed");
	}

	kq = kqueue();
	if (kq == -1)
	{
		close(server_fd);
		throw std::runtime_error("Failed to create kqueue");
	}

	struct kevent	ev;
	EV_SET(&ev, server_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	if (xKevent(kq, &ev, 1, NULL, 0, NULL) == -1)
	{
		close(server_fd);
		close(kq);
		throw std::runtime_error("Failed to add server socket event to kqueue");
	}
}

Server::~Server(void)
{
	if (server_fd != -1)
	{
		close(server_fd);
	}
	if (kq != -1)
	{
		close(kq);
	}
}

void Server::run_server(void)
{
	while (true)
	{
		waitEvent();
	}
}

void Server::waitEvent(void)
{
	const int		MAX_EVENTS = 10;
	struct kevent	events[MAX_EVENTS];
	int				nev = xKevent(kq, NULL, 0, events, MAX_EVENTS, NULL); //딜레이 설정 필요

	for (int i = 0; i < nev; i++)
	{
		if (events[i].filter == EVFILT_READ)
		{
			if (events[i].ident == static_cast<uintptr_t>(server_fd))
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
	struct sockaddr_in	client_addr;
	socklen_t			client_len = sizeof(client_addr);
	int					client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);

	if (client_fd >= 0)
	{
		fcntl(client_fd, F_SETFL, O_NONBLOCK);

		struct kevent	ev;
		EV_SET(&ev, client_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		xKevent(kq, &ev, 1, NULL, 0, NULL);
	}
}

void Server::handleRead(int fd)
{
}

void Server::prepareWrite(int fd, const char* data, size_t length)
{
	write_buffers[fd].append(data, length);

	struct kevent	ev;
	EV_SET(&ev, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
	xKevent(kq, &ev, 1, NULL, 0, NULL);
}

void Server::handleWrite(int fd)
{
	std::string&	buffer = write_buffers[fd];

	if (!buffer.empty())
	{
		ssize_t	bytes_sent = write(fd, buffer.c_str(), buffer.size());
		if (bytes_sent > 0)
		{
			buffer.erase(0, bytes_sent);
		}
		if (buffer.empty())
		{
			struct kevent	ev;
			EV_SET(&ev, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
			xKevent(kq, &ev, 1, NULL, 0, NULL);
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

	int xKevent(int kq, const struct kevent *changelist, int nchanges,
			struct kevent *eventlist, int nevents, const struct timespec *timeout)
	{
		int	retval = kevent(kq, changelist, nchanges, eventlist, nevents, timeout);
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
