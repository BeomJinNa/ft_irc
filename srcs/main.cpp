#include <iostream>
#include <sstream>
#include "Server.hpp"
#include "Command.hpp"

int	main(int argc, char* argv[])
{
	if (argc != 3)
	{
		std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
		return (1);
	}

	int	port;
	{
		std::istringstream	iss(argv[1]);
		iss >> port;
		if (iss.fail())
		{
			std::cerr<< "parsing failed" << std::endl;
			return (1);
		}

		if (port < 0 || port > 65535)
		{
			std::cerr << "Invalid port number" << std::endl;
			return (0);
		}
	}
	std::string	password = argv[2];

	Server*	ircServer;
	try
	{
		ircServer = new Server(port);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Server initialization failed: " << e.what() << std::endl;
		return (1);
	}

//	Command::RegisterCommand("KICK",	hookFunctionKick);
//	Command::RegisterCommand("INVITE",	hookFunctionInvite);
//	Command::RegisterCommand("TOPIC",	hookFunctionTopic);
//	Command::RegisterCommand("MODE",	hookFunctionMode);

	if (ircServer)
	{
		ircServer->SetServerPassword(password);
		ircServer->RunServer();
	}

	delete ircServer;

	return (0);
}
