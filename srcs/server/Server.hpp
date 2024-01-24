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

class Server
{
	public:
		Server(int port);
		~Server(void);

		void run_server(void);
		void prepareWrite(int fd, const char* data, size_t length);
	private:
		Server(void);
		Server(const Server& source);
		Server&	operator=(const Server& source);

		void waitEvent(void);
		void acceptConnection(void);
		void handleRead(int fd);
		void handleWrite(int fd);

		int							server_fd;
		int							kq;
		std::map<int, std::string>	write_buffers;
};
#endif

