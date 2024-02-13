#include <climits>
#include <sstream>
#include <string>
#include <vector>
#include "Message.hpp"
#include "UserDB.hpp"
#include "ChannelDB.hpp"
#include "ChannelMode.hpp"
#include "ErrorCodes.hpp"

typedef std::vector<std::string>	Tokens;

namespace
{
	void	applyUserMode(const Message& message);
	void	applyChannelMode(const Message& message);
	bool	divideCommands(Tokens* vec, const std::string& commands, int userId);
	bool	checkParameters(const Tokens& command, const Tokens& parameter,
							int userId, int channelId);
	void	executeChannelMode(const Tokens& command, const Tokens& parameter,
							int userId, int channelId);
}

void	HookFunctionMode(const Message& message)
{

	if (message.GetParameters().size() < 1)
	{
		UserDB::GetInstance().SendErrorMessageToUser("Not enough parameters for MODE command",
													 message.GetUserId(), M_ERR_NEEDMOREPARAMS,
													 message.GetUserId());
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
	void	applyUserMode(const Message& message)
	{
		(void)message;
	}

	void	applyChannelMode(const Message& message)
	{
		UserDB&		userDB = UserDB::GetInstance();
		ChannelDB&	channelDB = ChannelDB::GetInstance();
		int			userId = message.GetUserId();

		if (message.GetParameters().size() < 2)
		{
			userDB.SendErrorMessageToUser("Not enough parameters for MODE command",
										  userId, M_ERR_NEEDMOREPARAMS, userId);
			return ;
		}
		std::string	channelName = message.GetParameters()[0];

		int	channelId = channelDB.GetChannelIdByName(channelName);
		if (channelId == -1)
		{
			userDB.SendErrorMessageToUser(channelDB.GetChannelName(channelId)
										+ " :No such channel",
										  userId, M_ERR_NOSUCHCHANNEL, userId);
			return ;
		}

		std::string command = message.GetParameters()[1];
		if (command[0] != '+' && command[0] != '-')
		{
			userDB.SendErrorMessageToUser(":Invalid command value", userId,
										  M_ERR_NEEDMOREPARAMS, userId);
			return ;
		}

		Tokens	commandVec;
		if (divideCommands(&commandVec, command, userId) == false)
		{
			return ;
		}

		if (channelDB.IsUserOperator(channelId, userId) == false)
		{
			userDB.SendErrorMessageToUser("#" + channelDB.GetChannelName(channelId)
										+ " :You're not channel operator",
										  userId, M_ERR_CHANOPRIVSNEEDED, userId);
			return ;
		}
		Tokens	parametersVec = Tokens(message.GetParameters().begin() + 1,
				message.GetParameters().end());
		if (checkParameters(commandVec, parametersVec, userId, channelId) == false)
		{
			return ;
		}

		executeChannelMode(commandVec, parametersVec, userId, channelId);//코드 실행
	}

	bool	divideCommands(Tokens* vec, const std::string& commands, int userId)
	{
		static const std::string	commandLUT = "itkol";
		std::string					sign(1, commands[0]);
		std::string					buffer;
		std::string					commandsNoSign = commands.substr(1);

		(*vec).clear();
		for (std::size_t i = 0; i < commandsNoSign.size(); ++i)
		{
			bool	exist = false;
			for (std::size_t j = 0; j < commandLUT.size(); ++j)
			{
				if (commandsNoSign[i] == commandLUT[j])
				{
					buffer = sign + commandsNoSign[i];
					(*vec).push_back(buffer);
					exist = true;
					break;
				}
			}
			if (exist == false)
			{
				(*vec).clear();
				UserDB::GetInstance().SendErrorMessageToUser(std::string(1, commandsNoSign[i])
														   + " :is unknown mode char to me",
															 userId, M_ERR_UNKNOWNMODE, userId);
				return (false);
			}
		}
		return (true);
	}

	bool	checkParameters(const Tokens& command, const Tokens& parameter,
			int userId, int channelId)
	{
		UserDB&		userDB = UserDB::GetInstance();
		ChannelDB&	channelDB = ChannelDB::GetInstance();

		Tokens::const_iterator	pit = parameter.begin();

		for (Tokens::const_iterator it = command.begin(); it != command.end(); ++it)
		{
			if (*it == "+k")
			{
				if (pit == parameter.end() || (*pit).empty())
				{
					userDB.SendErrorMessageToUser("Not enough parameters for MODE command",
												  userId, M_ERR_NEEDMOREPARAMS, userId);
					return (false);
				}
				++pit;
			}
			else if (*it == "-k")
			{
				if ((channelDB.GetChannelFlag(channelId) & M_FLAG_CHANNEL_PASSWORD_CHECK_ON) == 0)
				{
					userDB.SendErrorMessageToUser("#" + channelDB.GetChannelName(channelId)
												+ " :No password is set",
												  userId, M_ERR_CHANOPRIVSNEEDED, userId);
					return (false);
				}
			}
			else if (*it == "+o")
			{
				if (pit == parameter.end() || (*pit).empty())
				{
					userDB.SendErrorMessageToUser("Not enough parameters for MODE command",
												  userId, M_ERR_NEEDMOREPARAMS, userId);
					return (false);
				}
				if (channelDB.IsUserInChannel(channelId, userDB.GetUserIdByNickName(*pit)) == false)
				{
					userDB.SendErrorMessageToUser(*pit + " " + channelDB.GetChannelName(channelId)
												+ " :They aren't on that channel",
												  userId, M_ERR_USERNOTINCHANNEL, userId);
					return (false);
				}
				++pit;
			}
			else if (*it == "-o")
			{
				if (pit == parameter.end() || (*pit).empty())
				{
					userDB.SendErrorMessageToUser("Not enough parameters for MODE command",
												  userId, M_ERR_NEEDMOREPARAMS, userId);
					return (false);
				}
				++pit;
			}
			else if (*it == "+l")
			{
				if (pit == parameter.end() || (*pit).empty())
				{
					userDB.SendErrorMessageToUser("Not enough parameters for MODE command",
												  userId, M_ERR_NEEDMOREPARAMS, userId);
					return (false);
				}

				std::istringstream	iss(*pit);
				long long	buffer;
				iss >> buffer;
				if (buffer <= 0 || buffer > std::numeric_limits<unsigned int>::max()
						|| buffer < channelDB.GetCurrentUsersInChannel(channelId))
				{
					userDB.SendErrorMessageToUser("#" + channelDB.GetChannelName(channelId)
												+ " :Invalid parameter value for +l",
												  userId, M_ERR_NEEDMOREPARAMS, userId);
					return (false);
				}
				++pit;
			}
			else if (*it == "-l")
			{
				if ((channelDB.GetChannelFlag(channelId) & M_FLAG_CHANNEL_MAX_USER_LIMIT_ON) == 0)
				{
					userDB.SendErrorMessageToUser("#" + channelDB.GetChannelName(channelId)
												+ " :User limit is net set",
												  userId, M_ERR_CHANOPRIVSNEEDED, userId);
					return (false);
				}
			}
			else if (*it == "-i")
			{
				if ((channelDB.GetChannelFlag(channelId) & M_FLAG_CHANNEL_INVITE_ONLY) == 0)
				{
					userDB.SendErrorMessageToUser("#" + channelDB.GetChannelName(channelId)
												+ " :Invite only mode is not set",
												  userId, M_ERR_CHANOPRIVSNEEDED, userId);
					return (false);
				}
			}
			else if (*it == "+t")
			{
				if ((channelDB.GetChannelFlag(channelId) & M_FLAG_CHANNEL_TOPIC_OPERATOR_ONLY) != 0)
				{
					userDB.SendErrorMessageToUser("#" + channelDB.GetChannelName(channelId)
												+ " :Topic operator only mode is already set",
												  userId, M_ERR_CHANOPRIVSNEEDED, userId);
					return (false);
				}
			}
			else if (*it == "-t")
			{
				if ((channelDB.GetChannelFlag(channelId) & M_FLAG_CHANNEL_TOPIC_OPERATOR_ONLY) == 0)
				{
					userDB.SendErrorMessageToUser("#" + channelDB.GetChannelName(channelId)
												+ " :Topic operator only mode is not set",
												  userId, M_ERR_CHANOPRIVSNEEDED, userId);
					return (false);
				}
			}
		}
		return (true);
	}

	void	executeChannelMode(const Tokens& command, const Tokens& parameter,
							   int userId, int channelId)
	{
		UserDB&		userDB = UserDB::GetInstance();
		ChannelDB&	channelDB = ChannelDB::GetInstance();
		std::string	channelName = channelDB.GetChannelName(channelId);

		Tokens::const_iterator	pit = parameter.begin();

		for (Tokens::const_iterator it = command.begin(); it != command.end(); ++it)
		{
			if (*it == "+i")
			{
				channelDB.AddChannelFlag(channelId, M_FLAG_CHANNEL_INVITE_ONLY);
				channelDB.SendFormattedMessageToChannel(
						"NOTICE " + channelName + " :" + userDB.GetNickName(userId)
						+ " has set mode +i on " + channelName
						,channelId, userId);
			}
			else if (*it == "-i")
			{
				channelDB.RemoveChannelFlag(channelId, M_FLAG_CHANNEL_INVITE_ONLY);
				channelDB.SendFormattedMessageToChannel(
						"NOTICE " + channelName + " :" + userDB.GetNickName(userId)
						+ " has removed mode +i on " + channelName
						,channelId, userId);
			}
			else if (*it == "+t")
			{
				channelDB.AddChannelFlag(channelId, M_FLAG_CHANNEL_TOPIC_OPERATOR_ONLY);
				channelDB.SendFormattedMessageToChannel(
						"NOTICE " + channelName + " :" + userDB.GetNickName(userId)
						+ " has set mode +t on " + channelName
						,channelId, userId);
			}
			else if (*it == "-t")
			{
				channelDB.RemoveChannelFlag(channelId, M_FLAG_CHANNEL_TOPIC_OPERATOR_ONLY);
				channelDB.SendFormattedMessageToChannel(
						"NOTICE " + channelName + " :" + userDB.GetNickName(userId)
						+ " has removed mode -t on " + channelName
						,channelId, userId);
			}
			else if (*it == "+k")
			{
				channelDB.AddChannelFlag(channelId, M_FLAG_CHANNEL_PASSWORD_CHECK_ON);
				channelDB.SetChannelPassword(channelId, *pit++);
				channelDB.SendFormattedMessageToChannel(
						"NOTICE " + channelName + " :" + userDB.GetNickName(userId)
						+ " has set mode +k on " + channelName
						,channelId, userId);
			}
			else if (*it == "-k")
			{
				channelDB.RemoveChannelFlag(channelId, M_FLAG_CHANNEL_PASSWORD_CHECK_ON);
				channelDB.SetChannelPassword(channelId, "");
				channelDB.SendFormattedMessageToChannel(
						"NOTICE " + channelName + " :" + userDB.GetNickName(userId)
						+ " has removed mode -k on " + channelName
						,channelId, userId);
			}
			else if (*it == "+o")
			{
				channelDB.AddOperatorIntoChannel(channelId, userDB.GetUserIdByNickName(*pit));
				channelDB.SendFormattedMessageToChannel(
						"NOTICE " + channelName + " :" + userDB.GetNickName(userId)
						+ " has given channel operator status to " + *pit + " on " + channelName
						,channelId, userId);
				++pit;
			}
			else if (*it == "-o")
			{
				channelDB.RemoveOperatorIntoChannel(channelId, userDB.GetUserIdByNickName(*pit++));
				channelDB.SendFormattedMessageToChannel(
						"NOTICE " + channelName + " :" + userDB.GetNickName(userId)
						+ " has taken channel operator status to " + *pit + " on " + channelName
						,channelId, userId);
			}
			else if (*it == "+l")
			{
				std::istringstream	iss(*pit);
				unsigned int		limit;

				iss >> limit;
				channelDB.AddChannelFlag(channelId, M_FLAG_CHANNEL_MAX_USER_LIMIT_ON);
				channelDB.SetMaxUsersInChannel(channelId, limit);
				channelDB.SendFormattedMessageToChannel(
						"NOTICE " + channelName + " :" + userDB.GetNickName(userId)
						+ " has set mode +l " + *pit + " on " + channelName
						,channelId, userId);
				++pit;
			}
			else if (*it == "-l")
			{
				channelDB.RemoveChannelFlag(channelId, M_FLAG_CHANNEL_MAX_USER_LIMIT_ON);
				channelDB.SetMaxUsersInChannel(channelId, std::numeric_limits<unsigned int>::max());
				channelDB.SendFormattedMessageToChannel(
						"NOTICE " + channelName + " :" + userDB.GetNickName(userId)
						+ " has removed mode -l on " + channelName
						,channelId, userId);
			}
		}
	}
}
