#include <string>
#include "User.hpp"

User::User(void)
	: mUserId(-1)
	, mSocketFd(-1)
	, mIsLoggedIn(false)
	, mIsUserNameSet(false)
	, mIsNickNameSet(false)
{
}

User::User(int userId, int socketFd)
	: mUserId(userId)
	, mSocketFd(socketFd)
	, mIsLoggedIn(false)
	, mIsUserNameSet(false)
	, mIsNickNameSet(false)
{
}

User::User(const User& source)
	: mUserId(source.mUserId)
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
		mUserId = source.mUserId;
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

void				User::SetUserId(int value) { mUserId = value; }
int					User::GetUserId(void) const { return (mUserId); }

void				User::SetSocketFd(int value) { mSocketFd = value; }
int					User::GetSocketFd(void) const { return (mSocketFd); }

void				User::SetLoginStatus(bool value) { mIsLoggedIn = value; }
bool				User::GetLoginStatus(void) const { return (mIsLoggedIn); }

void				User::SetUserName(const std::string& name)
{
	mIsUserNameSet = true;
	mUserName = name;
}

std::string&		User::GetUserName(void) { return (mUserName); }
const std::string&	User::GetUserName(void) const { return (mUserName); }

void				User::SetNickName(const std::string& name)
{
	mIsNickNameSet = true;
	mNickName = name;
}
std::string&		User::GetNickName(void) { return (mNickName); }
const std::string&	User::GetNickName(void) const { return (mNickName); }

void				User::SetFlagUserNameSet(bool value) { mIsUserNameSet = value; }
bool				User::GetFlagUserNameSet(void) const { return (mIsUserNameSet); }

void				User::SetFlagNickNameSet(bool value) { mIsNickNameSet = value; }
bool				User::GetFlagNickNameSet(void) const { return (mIsNickNameSet); }

std::vector<int>	User::GetChannelList(void) const
{
	return (std::vector<int>(mJoinedChannels.begin(), mJoinedChannels.end()));
}

void	User::AddChannelInJoinnedList(int channelId)
{
	mJoinedChannels.insert(channelId);
}

bool	User::RemoveChannelInJoinnedList(int channelId)
{
	std::set<int>::iterator	it = mJoinedChannels.find(channelId);

	if (it == mJoinedChannels.end())
	{
		return false;
	}
	mJoinedChannels.erase(it);
	return true;
}
