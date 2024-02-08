#include "Channel.hpp"
#include "UserDB.hpp"

Channel::Channel(void)
	: mChannelId(-1)
	, mChannelMode(0)
	, mMaxActiveUsers(0)
	, mCurrentActiveUsers(0)
{
}

Channel::Channel(int channelId, const std::string& channelName)
	: mChannelId(channelId)
	, mName(channelName)
	, mChannelMode(0)
	, mMaxActiveUsers(0)
	, mCurrentActiveUsers(0)
{
}

Channel::Channel(const Channel& source)
	: mChannelId(source.mChannelId)
	, mName(source.mName)
	, mChannelMode(source.mChannelMode)
	, mMaxActiveUsers(source.mMaxActiveUsers)
	, mCurrentActiveUsers(source.mCurrentActiveUsers)
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
		mChannelId			= source.mChannelId;
		mName				= source.mName;
		mChannelMode		= source.mChannelMode;
		mMaxActiveUsers		= source.mMaxActiveUsers;
		mCurrentActiveUsers	= source.mCurrentActiveUsers;
		mPassword			= source.mPassword;
		mTopic				= source.mTopic;
		mActiveUserList		= source.mActiveUserList;
		mOperatorList		= source.mOperatorList;
		mBanUserList		= source.mBanUserList;
	}

	return (*this);
}

Channel::~Channel(void)
{
}

bool	Channel::SetMaxActiveUsers(unsigned int maxUsers)
{
	if (mCurrentActiveUsers > maxUsers)
	{
		return (false);
	}
	mMaxActiveUsers = maxUsers;
	return (true);
}

unsigned int	Channel::GetMaxActiveUsers(void) const { return (mMaxActiveUsers); }
unsigned int	Channel::GetCurrentActiveUsers(void) const { return (mCurrentActiveUsers); }

bool	Channel::AddActiveUser(int userId)
{
	if (mCurrentActiveUsers < mMaxActiveUsers)
	{
		if (IsUserBanned(userId))
		{
			return (false);
		}
		std::pair<DB::iterator, bool>	it =  mActiveUserList.insert(userId);
		if (it.second == false) //이미 채널에 존재할 경우
		{
			return (true);
		}

		++mCurrentActiveUsers;
		return (true);
	}
	return (false);
}

void	Channel::RemoveUserData(int userId)
{
	RemoveActiveUser(userId);
	RemoveOperator(userId);
	RemoveInvitedUser(userId);
	RemoveBanUser(userId);
}

void	Channel::RemoveActiveUser(int userId)
{
	DB::iterator	it = mActiveUserList.find(userId);

	if (it == mActiveUserList.end())
	{
		return ;
	}
	mActiveUserList.erase(userId);
	--mCurrentActiveUsers;
}

bool	Channel::IsUserActive(int userID) const
{
	DB::iterator	it = mActiveUserList.find(userID);

	if (it == mActiveUserList.end())
	{
		return (false);
	}
	return (true);
}

bool	Channel::AddOperator(int userId)
{
	if (IsUserBanned(userId))
	{
		return (false);
	}
	mOperatorList.insert(userId);
	return (true);
}

void	Channel::RemoveOperator(int userId)
			{ mOperatorList.erase(userId); }

bool	Channel::IsUserOperator(int userID) const
{
	DB::iterator	it = mOperatorList.find(userID);

	if (it == mOperatorList.end())
	{
		return (false);
	}
	return (true);
}

void	Channel::AddInvitedUser(int userId)
{
	if (IsUserBanned(userId))
	{
		RemoveBanUser(userId);
	}
	mInvitedUserList.insert(userId);
}

void	Channel::RemoveInvitedUser(int userId)
			{ mInvitedUserList.erase(userId); }

bool	Channel::IsUserInvited(int userID) const
{
	DB::iterator	it = mInvitedUserList.find(userID);

	if (it == mInvitedUserList.end())
	{
		return (false);
	}
	return (true);
}

void	Channel::AddBanUser(int userId)
{
	mBanUserList.insert(userId);
	RemoveActiveUser(userId);
	RemoveInvitedUser(userId);
}

void	Channel::RemoveBanUser(int userId)
			{ mBanUserList.erase(userId); }

bool	Channel::IsUserBanned(int userID) const
{
	DB::iterator	it = mBanUserList.find(userID);

	if (it == mBanUserList.end())
	{
		return (false);
	}
	return (true);
}

void				Channel::SetChannelId(int channelId)
						{ mChannelId = channelId; }
unsigned int		Channel::GetChannelId(void) const
						{ return (mChannelId); }
void				Channel::SetChannelName(const std::string& name)
						{ mName = name; }
std::string&		Channel::GetChannelName(void){ return (mName); }
const std::string&	Channel::GetChannelName(void) const { return (mName); }
void				Channel::SetChannelMode(unsigned int mode)
						{ mChannelMode = mode; }
unsigned int		Channel::GetChannelMode(void) const
						{ return (mChannelMode); }
void				Channel::SetPassword(const std::string& password)
						{ mPassword = password; }
std::string&		Channel::GetPassword(void){ return (mPassword); }
const std::string&	Channel::GetPassword(void) const { return (mPassword); }
void				Channel::SetTopic(const std::string& topic) { mTopic = topic; }
std::string&		Channel::GetTopic(void) { return (mTopic); }
const std::string&	Channel::GetTopic(void) const { return (mTopic); }

std::vector<int>	Channel::GetActiveUserList(void) const
{
    return (std::vector<int>(mActiveUserList.begin(), mActiveUserList.end()));
}

std::vector<int>	Channel::GetOperatorList(void) const
{
    return (std::vector<int>(mOperatorList.begin(), mOperatorList.end()));
}

std::vector<int>	Channel::GetInvitedUserList(void) const
{
    return (std::vector<int>(mInvitedUserList.begin(), mInvitedUserList.end()));
}

std::vector<int>	Channel::GetBanUserList(void) const
{
    return (std::vector<int>(mBanUserList.begin(), mBanUserList.end()));
}
