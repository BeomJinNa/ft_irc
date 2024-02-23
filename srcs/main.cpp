#include <iostream>
#include <sstream>
#include "Server.hpp"
#include "UserDB.hpp"
#include "ChannelDB.hpp"
#include "Command.hpp"
#include "hooks.hpp"

namespace
{
	int		parsePortNumber(char* argv[]);
	bool	initServer(Server** server, int port);
	bool	initUserDatabase(UserDB** Database);
	bool	initChannelDatabase(ChannelDB** Database);
	void	addHooks(void);
	void 	signalHandler(int signum);
}

int	main(int argc, char* argv[])
{
	if (argc != 3)
	{
		std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
		return (1);
	}

	std::string	password = argv[2];
	bool		errorOccured;

	int	port = parsePortNumber(argv);
	if (port < 0)
	{
		return (1);
	}

	Server*	ircServer = NULL;
	errorOccured = initServer(&ircServer, port);
	if (errorOccured)
	{
		return (1);
	}

	UserDB*	userDB = NULL;
	errorOccured = initUserDatabase(&userDB);
	if (errorOccured)
	{
		delete ircServer;
		return (1);
	}

	ChannelDB*	channelDB = NULL;
	errorOccured = initChannelDatabase(&channelDB);
	if (errorOccured)
	{
		delete userDB;
		delete ircServer;
		return (1);
	}

	addHooks();

	ircServer->SetServerPassword(password);
	signal(SIGINT, signalHandler);
	signal(SIGQUIT, signalHandler);
	try
	{
		ircServer->RunServer();
	}
	catch (std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		Server::GetInstance().CloseAllClientConnection();
		delete channelDB;
		delete userDB;
		delete ircServer;
		return (1);
	}


	Server::GetInstance().CloseAllClientConnection();
	delete channelDB;
	delete userDB;
	delete ircServer;

	return (0);
}

namespace
{
	int	parsePortNumber(char* argv[])
	{
		int	port;
		{
			std::istringstream	iss(argv[1]);
			iss >> port;
			if (iss.fail())
			{
				std::cerr<< "parsing failed" << std::endl;
				return (-1);
			}

			if (port < 0 || port > 65535)
			{
				std::cerr << "Invalid port number" << std::endl;
				return (-1);
			}
		}
		return (port);
	}

	bool	initServer(Server** server, int port)
	{
		try
		{
			*server = new Server(port);
		}
		catch (const std::exception& e)
		{
			std::cerr << "Server initialization failed: " << e.what() << std::endl;
			return (true);
		}

		(*server)->DoNothing();
		return (false);
	}

	bool	initUserDatabase(UserDB** Database)
	{
		try
		{
			*Database = new UserDB();
		}
		catch (const std::exception& e)
		{
			std::cerr << "User Database initialization failed: "
					  << e.what() << std::endl;
			return (true);
		}

		(*Database)->DoNothing();
		return (false);
	}

	bool	initChannelDatabase(ChannelDB** Database)
	{
		try
		{
			*Database = new ChannelDB();
		}
		catch (const std::exception& e)
		{
			std::cerr << "Channel Database initialization failed: "
					  << e.what() << std::endl;
			return (true);
		}

		(*Database)->DoNothing();
		return (false);
	}

	void	addHooks(void)
	{
		Command::RegisterCommand("QUIT", HookFunctionQuit);
		Command::RegisterCommand("USER", HookFunctionUser);
		Command::RegisterCommand("NICK", HookFunctionNick);
		Command::RegisterCommand("PRIVMSG", HookFunctionPrivmsg);
		Command::RegisterCommand("PASS", HookFunctionPass);
		Command::RegisterCommand("PING", HookFunctionPing);
		Command::RegisterCommand("PART", HookFunctionPart);
		Command::RegisterCommand("JOIN", HookFunctionJoin);
		Command::RegisterCommand("TOPIC", HookFunctionTopic);
		Command::RegisterCommand("KICK", HookFunctionKick);
		Command::RegisterCommand("INVITE", HookFunctionInvite);
		Command::RegisterCommand("MODE", HookFunctionMode);
		Command::RegisterCommand("CAP", HookFunctionCap);
	}

	void signalHandler(int signum)
	{
		if (signum == SIGINT || signum == SIGQUIT)
		{
			std::cout << "Server is shutting down..." << std::endl;
			Server::GetInstance().CloseAllClientConnection();
			delete &ChannelDB::GetInstance();
			delete &UserDB::GetInstance();
			delete &Server::GetInstance();
			exit(signum);
		}
	}
}
