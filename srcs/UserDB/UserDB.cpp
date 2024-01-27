#include "UserDB.hpp"
#include "User.hpp"
#include "Server.hpp"

namespace
{
	UserDB*	TouchInstanceData(UserDB* address);
}

UserDB::UserDB(void)
{
	TouchInstanceData(this);
}

UserDB::~UserDB(void) {}

void	UserDB::ConnectUser(int clientFd)
{
	if (clientFd >= 0)
	{
		mDataBase[clientFd].SetClientFd(clientFd);
	}
}

void	UserDB::RemoveUserData(int clientFd)
{
	const DB::iterator&	it = mDataBase.find(clientFd);

	if (it != mDataBase.end())
	{
		mReferenceTableNickName.erase(it->second.GetNickName());
		mReferenceTableUserName.erase(it->second.GetUserName());
		mDataBase.erase(it);
	}
}

void	UserDB::DisconnectUser(int clientFd)
{
	RemoveUserData(clientFd);
	//TODO
	//Channel DB에 해당 유저의 자원 회수 요청이 이 부분에 추가되어야 함

	Server& ircServer = Server::GetInstance();
	ircServer.CloseClientConnection(clientFd);
}

int	UserDB::GetClientFdByUserName(const std::string& userName) const
{
	const RefDB::const_iterator&	it
		= mReferenceTableUserName.find(userName);

	if (it != mReferenceTableUserName.end())
	{
		return (it->second);
	}

	return (-1);
}

int	UserDB::GetClientFdByNickName(const std::string& nickName) const
{
	const RefDB::const_iterator&	it
		= mReferenceTableNickName.find(nickName);

	if (it != mReferenceTableNickName.end())
	{
		return (it->second);
	}

	return (-1);
}

void	UserDB::SetLoginStatus(int clientFd, bool value)
{
	const DB::iterator&	it = mDataBase.find(clientFd);

	if (it != mDataBase.end())
	{
		it->second.SetLoginStatus(value);
	}
}

bool	UserDB::GetLoginStatus(int clientFd) const
{
	const DB::const_iterator&	it = mDataBase.find(clientFd);

	if (it != mDataBase.end())
	{
		return (it->second.GetLoginStatus());
	}
	return (false);
}

void	UserDB::SetUserName(int clientFd, const std::string& name)
{
	const DB::iterator&	it = mDataBase.find(clientFd);

	if (it != mDataBase.end())
	{
		it->second.SetUserName(name);
		it->second.SetFlagUserNameSet(true);
		if (clientFd >= 0)
		{
			mReferenceTableUserName[name] = clientFd;
		}
	}
}

const std::string&	UserDB::GetUserName(int clientFd) const
{
	const DB::const_iterator&	it = mDataBase.find(clientFd);

	if (it != mDataBase.end())
	{
		return (it->second.GetUserName());
	}
	throw std::out_of_range("Can not get username that doesn't exist");
}


void	UserDB::SetNickName(int clientFd, const std::string& name)
{
	const DB::iterator&	it = mDataBase.find(clientFd);

	if (it != mDataBase.end())
	{
		it->second.SetNickName(name);
		it->second.SetFlagNickNameSet(true);
		if (clientFd >= 0)
		{
			mReferenceTableNickName[name] = clientFd;
		}
	}
}

const std::string&	UserDB::GetNickName(int clientFd) const
{
	const DB::const_iterator&	it = mDataBase.find(clientFd);

	if (it != mDataBase.end())
	{
		return (it->second.GetNickName());
	}
	throw std::out_of_range("Can not get nickname that doesn't exist");
}

bool	UserDB::IsUserAuthorized(int clientFd) const
{
	const DB::const_iterator&	it = mDataBase.find(clientFd);

	if (it != mDataBase.end())
		return (it->second.GetFlagUserNameSet()
				&& it->second.GetFlagNickNameSet());
	return (false);
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
