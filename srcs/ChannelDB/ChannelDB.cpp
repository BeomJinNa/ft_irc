#include <cstddef>
#include "ChannelDB.hpp"
#include "Channel.hpp"
#include "UserDB.hpp"
#include "ChannelMode.hpp"

namespace
{
	ChannelDB*	TouchInstanceData(ChannelDB* address);
}

ChannelDB::ChannelDB(void)
{
	TouchInstanceData(this);
}

ChannelDB::~ChannelDB(void) {}

int	ChannelDB::CreateChannel(const std::string& name)
{
	RefDB::iterator	it = mReferenceTableName.find(name);

	if (it != mReferenceTableName.end())
	{
		return (-1);
	}

	int	Id = mIndex.GetNewIndex();

	if (Id < 0)
	{
		return (-1);
	}

	std::pair<DB::iterator, bool>	result
		= mDataBase.insert(std::make_pair(Id, Channel(Id, name)));

	if (result.second)
	{
		mReferenceTableName[name] = Id;
		return (Id);
	}
	return (-1);
}

void	ChannelDB::DeleteChannel(int channelId)
{
	DB::iterator	it = mDataBase.find(channelId);

	if (it == mDataBase.end())
	{
		return ;
	}
	mIndex.DeactivateIndex(it->second.GetChannelId());

	mDataBase.erase(it);
	UserDB::GetInstance().RemoveChannelInAllUsers(channelId);
}

bool	ChannelDB::AddUserInChannel(int channelId, int userId)
{
	DB::iterator	it = mDataBase.find(channelId);

	if (it == mDataBase.end())
	{
		return (false);
	}
	bool IsUserInChannel = it->second.AddActiveUser(userId);
	if (IsUserInChannel)
	{
		//User가 가지고 있는 입장 채널 목록은 std::map으로 관리되므로
		//이미 입장되어 있는 상태에서도 중복 추가되는 문제가 발생하지 않음.
		UserDB::GetInstance().WriteChannelInUserData(userId, channelId);
	}

	return (IsUserInChannel);
}

void	ChannelDB::RemoveUserInChannel(int channelId, int userId)
{
	DB::iterator	it = mDataBase.find(channelId);

	if (it == mDataBase.end())
	{
		return ;
	}
	it->second.RemoveUserData(userId);
	UserDB::GetInstance().RemoveChannelInUserList(userId, channelId);
}

void	ChannelDB::DeleteUserInAllChannels(int userId)
{
	for (DB::iterator it = mDataBase.begin(); it != mDataBase.end(); ++it)
	{
		it->second.RemoveUserData(userId);
	}
}

int	ChannelDB::GetChannelIdByName(const std::string& name) const
{
	RefDB::const_iterator	it = mReferenceTableName.find(name);

	if (it == mReferenceTableName.end())
	{
		return (-1);
	}
	return (it->second);
}

void	ChannelDB::SetChannelFlag(int channelId, unsigned int flag)
{
	DB::iterator	it = mDataBase.find(channelId);

	if (it == mDataBase.end())
	{
		return ;
	}
	it->second.SetChannelId(flag);
}

unsigned int	ChannelDB::GetChannelFlag(int channelId) const
{
	DB::const_iterator	it = mDataBase.find(channelId);

	if (it == mDataBase.end())
	{
		return (M_FLAG_CHANNEL_DOES_NOT_EXIST);
	}
	return (it->second.GetChannelMode());
}

ChannelDB& ChannelDB::GetInstance(void)
{
	ChannelDB*	output = TouchInstanceData(NULL);

	if (output == NULL)
		throw std::runtime_error("Channel Database Not Found");
	return (*output);
}

namespace
{
	ChannelDB*	TouchInstanceData(ChannelDB* address)
	{
		static ChannelDB*	ChannelDBGlobal = NULL;

		if (address != NULL)
		{
			ChannelDBGlobal = address;
			return (NULL);
		}

		return (ChannelDBGlobal);
	}
}

void	ChannelDB::DoNothing(void) const {}
ChannelDB::ChannelDB(const ChannelDB& source) { (void)source; }
ChannelDB&	ChannelDB::operator=(const ChannelDB& source)
{ if (this != &source) {} return (*this); }
