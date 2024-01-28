#include <string>
#include <algorithm>
#include "User.hpp"

User::User(void)
	: mClientFd(-1)
	, mSocketFd(-1)
	, mIsLoggedIn(false)
	, mIsUserNameSet(false)
	, mIsNickNameSet(false)
{
}

User::User(int clientFd, int socketFd)
	: mClientFd(clientFd)
	, mSocketFd(socketFd)
	, mIsLoggedIn(false)
	, mIsUserNameSet(false)
	, mIsNickNameSet(false)
{
}

User::User(const User& source)
	: mClientFd(source.mClientFd)
	, mSocketFd(source.mSocketFd)
	, mIsLoggedIn(source.mIsLoggedIn)
	, mIsUserNameSet(source.mIsUserNameSet)
	, mIsNickNameSet(source.mIsNickNameSet)
	, mUserName(source.mUserName)
	, mNickName(source.mNickName)
	, mJoinedChannels(source.mJoinedChannels)
{
}

User&	User::operator=(const User& source)
{
	if (this != &source)
	{
		mClientFd = source.mClientFd;
		mSocketFd = source.mSocketFd;
		mIsLoggedIn = source.mIsLoggedIn;
		mIsUserNameSet = source.mIsUserNameSet;
		mIsNickNameSet = source.mIsNickNameSet;
		mUserName = source.mUserName;
		mNickName = source.mNickName;
		mJoinedChannels = source.mJoinedChannels;
	}

	return (*this);
}

User::~User(void) {}

void				User::SetClientFd(int value) { mClientFd = value; }
int					User::GetClientFd(void) const { return (mClientFd); }

void				User::SetSocketFd(int value) { mSocketFd = value; }
int					User::GetSocketFd(void) const { return (mSocketFd); }

void				User::SetLoginStatus(bool value) { mIsLoggedIn = value; }
bool				User::GetLoginStatus(void) const { return (mIsLoggedIn); }

void				User::SetUserName(const std::string& name) { mUserName = name; }
std::string&		User::GetUserName(void) { return (mUserName); }
const std::string&	User::GetUserName(void) const { return (mUserName); }

void				User::SetNickName(const std::string& name) { mNickName = name; }
std::string&		User::GetNickName(void) { return (mNickName); }
const std::string&	User::GetNickName(void) const { return (mNickName); }

void				User::SetFlagUserNameSet(bool value) { mIsUserNameSet = value; }
bool				User::GetFlagUserNameSet(void) const { return (mIsUserNameSet); }

void				User::SetFlagNickNameSet(bool value) { mIsNickNameSet = value; }
bool				User::GetFlagNickNameSet(void) const { return (mIsNickNameSet); }

void	User::AddChannelInJoinnedList(int channelId)
{
	std::set<int>::iterator	it = std::find(mJoinedChannels.begin(),
										   mJoinedChannels.end(),
									  	   channelId);

	if (it == mJoinedChannels.end())
	{
		return ;
	}
	mJoinedChannels.insert(channelId);
}

void	User::RemoveChannelInJoinnedList(int channelId)
{
	std::set<int>::iterator	it = std::find(mJoinedChannels.begin(),
										   mJoinedChannels.end(),
										   channelId);

	if (it == mJoinedChannels.end())
	{
		return ;
	}
	mJoinedChannels.erase(it);
}
