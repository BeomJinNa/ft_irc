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
	ircServer->RunServer();

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
		Command::RegisterCommand("PART", HookFunctionPart);
		Command::RegisterCommand("PASS", HookFunctionPass);
		Command::RegisterCommand("USER", HookFunctionUser);
		Command::RegisterCommand("NICK", HookFunctionNick);
		Command::RegisterCommand("JOIN", HookFunctionJoin);
		Command::RegisterCommand("KICK", HookFunctionKick);
		Command::RegisterCommand("MODE", HookFunctionMode);
		Command::RegisterCommand("TOPIC", HookFunctionTopic);
		Command::RegisterCommand("PING", HookFunctionPing);
		Command::RegisterCommand("INVITE", HookFunctionInvite);
	}
}
