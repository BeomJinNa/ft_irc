#include <sstream>
#include <iomanip>
#include "UserDB.hpp"
#include "User.hpp"
#include "Server.hpp"
#include "ChannelDB.hpp"

namespace
{
	UserDB*	TouchInstanceData(UserDB* address);
}

UserDB::UserDB(void)
	: mIndex(100000)
{
	TouchInstanceData(this);
}

UserDB::~UserDB(void) {}

bool	UserDB::ConnectUser(int socketFd)
{
	if (socketFd >= 0)
	{
		int userId = mIndex.GetNewIndex();
		if (userId < 0)
		{
			return (false);
		}
		mDataBase[userId].SetUserId(userId);
		mDataBase[userId].SetSocketFd(socketFd);
		mReferenceTableSocket[socketFd] = userId;
		return (true);
	}
	return (false);
}

int	UserDB::RemoveUserData(int userId)
{
	const DB::iterator&	it = mDataBase.find(userId);
	int					socketFd;

	if (it != mDataBase.end())
	{
		socketFd = it->second.GetSocketFd();

		mReferenceTableNickName.erase(it->second.GetNickName());
		mReferenceTableUserName.erase(it->second.GetUserName());
		mReferenceTableSocket.erase(it->second.GetSocketFd());
		mDataBase.erase(it);
		mIndex.DeactivateIndex(userId);

		ChannelDB::GetInstance().DeleteUserInAllChannels(userId);

		return (socketFd);
	}
	return (-1);
}

void	UserDB::DisconnectUser(int userId)
{
	int	socketFd = RemoveUserData(userId);

	if (socketFd >= 0)
	{
		Server& ircServer = Server::GetInstance();
		ircServer.CloseClientConnection(socketFd);
	}
}

bool	UserDB::IsUserIdValid(int userId) const
{
	DB::const_iterator	it = mDataBase.find(userId);

	if (it == mDataBase.end())
	{
		return (false);
	}
	return (true);
}

void	UserDB::WriteChannelInUserData(int userId, int channelId)
{
	DB::iterator	it = mDataBase.find(userId);

	if (it == mDataBase.end())
	{
		return ;
	}
	it->second.AddChannelInJoinnedList(channelId);
}

UserDB::ChannelList	UserDB::GetJoinnedChannelList(int userId) const
{
	DB::const_iterator	it = mDataBase.find(userId);

	if (it == mDataBase.end())
	{
		return (ChannelList());
	}
	return (it->second.GetChannelList());
}

bool	UserDB::AddChannelInUserList(int userId, int channelId)
{
	bool	IsUserInChannel
	= ChannelDB::GetInstance().AddUserIntoChannel(channelId, userId);

	return (IsUserInChannel);
}

void	UserDB::RemoveChannelInUserList(int userId, int channelId)
{
	DB::iterator	it = mDataBase.find(userId);

	if (it == mDataBase.end())
	{
		return ;
	}
	it->second.RemoveChannelInJoinnedList(channelId);
	ChannelDB::GetInstance().RemoveUserIntoChannel(channelId, userId);
}

void	UserDB::RemoveChannelInAllUsers(int channelId)
{
	for (DB::iterator it = mDataBase.begin(); it != mDataBase.end(); ++it)
	{
		it->second.RemoveChannelInJoinnedList(channelId);
	}
}

int	UserDB::GetUserIdByUserName(const std::string& userName) const
{
	const RefDB::const_iterator&	it
		= mReferenceTableUserName.find(userName);

	if (it != mReferenceTableUserName.end())
	{
		return (it->second);
	}

	return (-1);
}

int	UserDB::GetUserIdByNickName(const std::string& nickName) const
{
	const RefDB::const_iterator&	it
		= mReferenceTableNickName.find(nickName);

	if (it != mReferenceTableNickName.end())
	{
		return (it->second);
	}

	return (-1);
}

int	UserDB::GetUserIdBySocketId(int socketId) const
{
	const SocketRefDB::const_iterator&	it
		= mReferenceTableSocket.find(socketId);

	if (it != mReferenceTableSocket.end())
	{
		return (it->second);
	}

	return (-1);
}

int	UserDB::GetSocketIdByUserId(int userId) const
{
	const DB::const_iterator&	it = mDataBase.find(userId);

	if (it == mDataBase.end())
	{
		return (-1);
	}
	return (it->second.GetSocketFd()); }

void	UserDB::SendMessageToUser(const std::string& message, int userId) const
{
	DB::const_iterator	it = mDataBase.find(userId);

	if (it == mDataBase.end())
	{
		return ;
	}
	Server::GetInstance().SendMessageToClient(it->second.GetSocketFd(),
											  message.c_str(), message.size());
}

void	UserDB::SendErrorMessageToUser(const std::string& message, int userId, int code) const
{
	Server&	serv = Server::GetInstance();
	UserDB&	uDB = UserDB::GetInstance();

	std::ostringstream	oss;
	oss << ":" << serv.GetHostAddress()
		<< " " << std::setw(3) << code
		<< " " << uDB.GetNickName(userId)
		<< " " << message;
	uDB.SendMessageToUser(oss.str(), userId);
}

void	UserDB::SendFormattedMessageToUser(const std::string& message, int userId) const
{
	Server&	serv = Server::GetInstance();
	UserDB&	uDB = UserDB::GetInstance();

	std::string	sendingMessage
		= ":" + uDB.GetNickName(userId)
		+ "!" + uDB.GetUserName(userId)
		+ "@" + serv.GetHostAddress();
		+ " " + message;
	uDB.SendMessageToUser(sendingMessage, userId);
}

void	UserDB::SetLoginStatus(int userId, bool value)
{
	const DB::iterator&	it = mDataBase.find(userId);

	if (it != mDataBase.end())
	{
		it->second.SetLoginStatus(value);
	}
}

bool	UserDB::GetLoginStatus(int userId) const
{
	const DB::const_iterator&	it = mDataBase.find(userId);

	if (it != mDataBase.end())
	{
		return (it->second.GetLoginStatus());
	}
	return (false);
}

bool	UserDB::IsUserAuthorized(int userId) const
{
	const DB::const_iterator&	it = mDataBase.find(userId);

	if (it != mDataBase.end())
		return (it->second.GetFlagUserNameSet()
				&& it->second.GetFlagNickNameSet());
	return (false);
}

void	UserDB::SetUserName(int userId, const std::string& name)
{
	const DB::iterator&	it = mDataBase.find(userId);

	if (it != mDataBase.end())
	{
		RefDB::iterator	rit = mReferenceTableUserName.find(name);
		if (rit != mReferenceTableUserName.end())
		{
			mReferenceTableUserName.erase(rit);
		}
		it->second.SetUserName(name);
		it->second.SetFlagUserNameSet(true);
		mReferenceTableUserName[name] = userId;
	}
}

std::string	UserDB::GetUserName(int userId) const
{
	const DB::const_iterator&	it = mDataBase.find(userId);

	if (it == mDataBase.end())
	{
		return ("");
	}
	return (it->second.GetUserName());
}


void	UserDB::SetNickName(int userId, const std::string& name)
{
	const DB::iterator&	it = mDataBase.find(userId);

	if (it != mDataBase.end())
	{
		RefDB::iterator	rit = mReferenceTableNickName.find(name);
		if (rit != mReferenceTableNickName.end())
		{
			mReferenceTableNickName.erase(rit);
		}
		it->second.SetNickName(name);
		it->second.SetFlagNickNameSet(true);
		mReferenceTableNickName[name] = userId;
	}
}

std::string	UserDB::GetNickName(int userId) const
{
	const DB::const_iterator&	it = mDataBase.find(userId);

	if (it == mDataBase.end())
	{
		return ("");
	}
	return (it->second.GetNickName());
}

UserDB& UserDB::GetInstance(void)
{
	UserDB*	output = TouchInstanceData(NULL);

	if (output == NULL)
		throw std::runtime_error("User Database Not Found");
	return (*output);
}

namespace
{
	UserDB*	TouchInstanceData(UserDB* address)
	{
		static UserDB*	UserDBGlobal = NULL;

		if (address != NULL)
		{
			UserDBGlobal = address;
			return (NULL);
		}

		return (UserDBGlobal);
	}
}

void	UserDB::DoNothing(void) const {}
UserDB::UserDB(const UserDB& source) { (void)source; }
UserDB&	UserDB::operator=(const UserDB& source)
{ if (this != &source) {} return (*this); }
