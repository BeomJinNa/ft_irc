#include "Channel.hpp"

Channel::Channel(void)
	: mChannelMode(0)
	, mMaxActiveUsers(0)
{
}

Channel::Channel(const std::string& channelName)
	: mName(channelName)
	, mChannelMode(0)
	, mMaxActiveUsers(0)
{
}

Channel::Channel(const Channel& source)
	: mName(source.mName)
	, mChannelMode(source.mChannelMode)
	, mMaxActiveUsers(source.mMaxActiveUsers)
	, mPassword(source.mPassword)
	, mTopic(source.mTopic)
	, mActiveUserList(source.mActiveUserList)
	, mOperatorList(source.mOperatorList)
	, mBanUserList(source.mBanUserList)
{
}

Channel&	Channel::operator=(const Channel& source)
{
	if (this != &source)
	{
		mName			= source.mName;
		mChannelMode	= source.mChannelMode;
		mMaxActiveUsers	= source.mMaxActiveUsers;
		mPassword		= source.mPassword;
		mTopic			= source.mTopic;
		mActiveUserList	= source.mActiveUserList;
		mOperatorList	= source.mOperatorList;
		mBanUserList	= source.mBanUserList;
	}

	return (*this);
}

Channel::~Channel(void)
{
	//모든 유저의 채널 리스트에서 현재 채널 삭제 요청
}


std::vector<int>	Channel::GetActiveUserList(void) const
{
    return (std::vector<int>(mActiveUserList.begin(), mActiveUserList.end()));
}

std::vector<int>	Channel::GetOperatorList(void) const
{
    return (std::vector<int>(mOperatorList.begin(), mOperatorList.end()));
}

std::vector<int>	Channel::GetBanUserList(void) const
{
    return (std::vector<int>(mBanUserList.begin(), mBanUserList.end()));
}

void				Channel::SetChannelMode(unsigned int mode) { mChannelMode = mode; }
unsigned int		Channel::GetChannelMode(void) const { return (mChannelMode); }

void				Channel::SetMaxActiveUsers(unsigned int maxUsers)
						{ mMaxActiveUsers = maxUsers; }
unsigned int		Channel::GetMaxActiveUsers(void) const { return (mMaxActiveUsers); }

void				Channel::SetPassword(const std::string& password) { mPassword = password; }
std::string&		Channel::GetPassword(void) { return (mPassword); }
const std::string&	Channel::GetPassword(void) const { return (mPassword); }

void				Channel::SetTopic(const std::string& topic) { mTopic = topic; }
std::string&		Channel::GetTopic(void) { return (mTopic); }
const std::string&	Channel::GetTopic(void) const { return (mTopic); }

void				Channel::AddActiveUser(int userId) { mActiveUserList.insert(userId); }
void				Channel::RemoveActiveUser(int userId) { mActiveUserList.erase(userId); }

void				Channel::AddOperator(int userId) { mOperatorList.insert(userId); }
void				Channel::RemoveOperator(int userId) { mOperatorList.erase(userId); }

void			Channel::AddBanUser(int userId) { mBanUserList.insert(userId); }
void			Channel::RemoveBanUser(int userId) { mBanUserList.erase(userId); }
