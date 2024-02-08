#include <sstream>
#include <string>
#include <vector>
#include "Server.hpp"
#include "UserDB.hpp"
#include "ChannelDB.hpp"
#include "Message.hpp"
typedef std::vector<std::string>	Tokens;

namespace
{
	void	sendMessage(int userId, int code, const std::string& message);
	void	applyUserMode(const Message& message);
	void	applyChannelMode(const Message& message);
	bool	divideCommands(Tokens* vec, const std::string& commands);
	bool	checkParameters(Tokens command, Tokens parameter);
}

void	HookFunctionMode(const Message& message)
{
	if (message.GetParameters().size() < 1)
	{
		//M_ERR_NEEDMOREPARAMS
		return ;
	}

	if (message.GetParameters()[0][0] != '#'
	 && message.GetParameters()[0][0] != '&')
	{
		applyUserMode(message);
	}
	else
	{
		applyChannelMode(message);
	}
}

namespace
{
	void	sendMessage(int userId, int code, const std::string& message)
	{
		Server&	serv = Server::GetInstance();
		UserDB&	uDB = UserDB::GetInstance();

		std::ostringstream	oss;
		oss << ":" << serv.GetHostAddress()
			<< " " << code << " " << uDB.GetUserName(userId)
			<< " " << message;
		uDB.SendMessageToUser(oss.str(), userId);
	}

	void	applyUserMode(const Message& message)
	{
	}

	void	applyChannelMode(const Message& message)
	{
		Server&		server = Server::GetInstance();
		UserDB&		userDB = UserDB::GetInstance();
		ChannelDB&	channelDB = ChannelDB::GetInstance();
		int			userId = message.GetUserId();

		if (message.GetParameters().size() < 2)
		{
			//M_ERR_NEEDMOREPARAMS
			return ;
		}
		std::string	channelName = message.GetParameters()[0].substr(1);

		int	channelId = channelDB.GetChannelIdByName(channelName);
		if (channelId == -1)
		{
			//M_ERR_NOSUCHCHANNEL
			return ;
		}

		std::string command = message.GetParameters()[1];
		if (command[0] != '+' && command[0] != '-')
		{
			//별도의 메시지 코드 없음
			return ;
		}

		Tokens	commandVec;
		if (divideCommands(&commandVec, command) == false)
		{
			//UNKNOWNMODE 에러
			return ;
		}

		Tokens	parametersVec = Tokens(message.GetParameters().begin() + 2,
									message.GetParameters().end());

		if (checkParameters(commandVec, parametersVec) == false)
		{
			return ;
		}
		while (itCommand != commandVec.end())
		{
			//코드 실행
		}
	}

	bool	divideCommands(Tokens* vec, const std::string& commands)
	{
		static const std::string	commandLUT = "itkol";
		char						sign = commands[0];
		std::string					buffer;

		commands.substr(1);
		(*vec).clear();
		for (std::size_t i = 0; i < commands.size(); ++i)
		{
			bool	exist = false;
			for (std::size_t j = 0; j < commandLUT.size(); ++j)
			{
				if (commands[i] == commandLUT[j])
				{
					buffer = sign + commands[i];
					(*vec).push_back(buffer);
					exist = true;
					break;
				}
			}
			if (exist == false)
			{
				(*vec).clear();
				return (false);
			}
		}
		return (true);
	}

	bool	checkParameters(const Tokens& command, const Tokens& parameter)
	{
		Tokens::const_iterator	pit = parameter.begin();

		for (Tokens::const_iterator it = command.begin(); it != command.end(); ++it)
		{
			if (*it == "+k")
			{
				if (pit == parameter.end() || (*pit).empty())
				{
					//M_ERR_NEEDMOREPARAMS
					return (false);
				}
				++pit;
				continue;
			}
			else if (*it == "+o")
			{
			}
			else if (*it == "-o")
			{
			}
		}
	}
}
