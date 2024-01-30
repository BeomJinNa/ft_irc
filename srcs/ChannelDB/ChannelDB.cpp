#include <stdexcept>
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

	int	channelId = mIndex.GetNewIndex();

	if (channelId < 0)
	{
		return (-1);
	}

	std::pair<DB::iterator, bool>	result
		= mDataBase.insert(std::make_pair(channelId,
										  Channel(channelId, name)));

	if (result.second)
	{
		mReferenceTableName[name] = channelId;
		return (channelId);
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
	mIndex.DeactivateIndex(channelId);

	mDataBase.erase(it);
	UserDB::GetInstance().RemoveChannelInAllUsers(channelId);
}

bool	ChannelDB::IsChannelIdValid(int channelId) const
{
	DB::const_iterator	it = mDataBase.find(channelId);

	if (it == mDataBase.end())
	{
		return (false);
	}
	return (true);
}

bool	ChannelDB::AddUserIntoChannel(int channelId, int userId)
{
	DB::iterator	it = mDataBase.find(channelId);

	if (it == mDataBase.end())
	{
		return (false);
	}
	bool isUserInChannel = it->second.AddActiveUser(userId);
	if (isUserInChannel)
	{
		//User가 가지고 있는 입장 채널 목록은 std::map으로 관리되므로
		//이미 입장되어 있는 상태에서도 중복 추가되는 문제가 발생하지 않음.
		UserDB::GetInstance().WriteChannelInUserData(userId, channelId);
	}

	return (isUserInChannel);
}

void	ChannelDB::RemoveUserIntoChannel(int channelId, int userId)
{
	DB::iterator	it = mDataBase.find(channelId);

	if (it == mDataBase.end())
	{
		return ;
	}
	it->second.RemoveUserData(userId);
	UserDB::GetInstance().RemoveChannelInUserList(userId, channelId);
}

bool	ChannelDB::AddOperatorIntoChannel(int channelId, int userId)
{
	DB::iterator	it = mDataBase.find(channelId);

	if (it == mDataBase.end())
	{
		return (false);
	}

	bool isUserOperator = it->second.AddOperator(userId);
	return (isUserOperator);
}

void	ChannelDB::RemoveOperatorIntoChannel(int channelId, int userId)
{
	DB::iterator	it = mDataBase.find(channelId);

	if (it == mDataBase.end())
	{
		return ;
	}
	it->second.RemoveOperator(userId);
}

bool	ChannelDB::IsUserOperator(int channelId, int userId) const
{
	DB::const_iterator	it = mDataBase.find(channelId);

	if (it == mDataBase.end())
	{
		return (false);
	}
	return (it->second.IsUserOperator(userId));
}

void	ChannelDB::AddBanIntoChannel(int channelId, int userId)
{
	DB::iterator	it = mDataBase.find(channelId);

	if (it == mDataBase.end())
	{
		return ;
	}
	it->second.AddBanUser(userId);
}

void	ChannelDB::RemoveBanIntoChannel(int channelId, int userId)
{
	DB::iterator	it = mDataBase.find(channelId);

	if (it == mDataBase.end())
	{
		return ;
	}
	it->second.RemoveBanUser(userId);
}

bool	ChannelDB::IsUserBanned(int channelId, int userId) const
{
	DB::const_iterator	it = mDataBase.find(channelId);

	if (it == mDataBase.end())
	{
		return (false);
	}
	return (it->second.IsUserBanned(userId));
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
	it->second.SetChannelMode(flag);
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

void	ChannelDB::AddChannelFlag(int channelId, unsigned int flag)
{
	SetChannelFlag(channelId, GetChannelFlag(channelId) | flag);
}

void	ChannelDB::RemoveChannelFlag(int channelId, unsigned int flag)
{
	SetChannelFlag(channelId, GetChannelFlag(channelId) & ~flag);
}

void	ChannelDB::SetChannelName(int channelId, const std::string& name)
{
	DB::iterator	it = mDataBase.find(channelId);

	if (it == mDataBase.end())
	{
		return ;
	}
	it->second.SetChannelName(name);
}

std::string	ChannelDB::GetChannelName(int channelId)
{
	DB::iterator	it = mDataBase.find(channelId);

	if (it == mDataBase.end())
	{
		return ("");
	}
	return (it->second.GetChannelName());
}

void	ChannelDB::SetChannelTopic(int channelId, const std::string& topic)
{
	DB::iterator	it = mDataBase.find(channelId);

	if (it == mDataBase.end())
	{
		return ;
	}
	it->second.SetTopic(topic);
}

std::string	ChannelDB::GetChannelTopic(int channelId)
{
	DB::iterator	it = mDataBase.find(channelId);

	if (it == mDataBase.end())
	{
		return ("");
	}
	return (it->second.GetTopic());
}

ChannelDB::UserList	ChannelDB::GetUserListInChannel(int channelId) const
{
	DB::const_iterator	it = mDataBase.find(channelId);

	if (it == mDataBase.end())
	{
		return (UserList());
	}
	return (it->second.GetActiveUserList());
}

ChannelDB::UserList	ChannelDB::GetOperatorListInChannel(int channelId) const
{
	DB::const_iterator	it = mDataBase.find(channelId);

	if (it == mDataBase.end())
	{
		return (UserList());
	}
	return (it->second.GetOperatorList());
}

ChannelDB::UserList	ChannelDB::GetBanListInChannel(int channelId) const
{
	DB::const_iterator	it = mDataBase.find(channelId);

	if (it == mDataBase.end())
	{
		return (UserList());
	}
	return (it->second.GetBanUserList());
}

void	ChannelDB::SendMessageToChannel(const std::string& message,
										int channelId) const
{
	DB::const_iterator	it = mDataBase.find(channelId);

	if (it == mDataBase.end())
	{
		return ;
	}

	UserList	list = it->second.GetActiveUserList();
	for (UserList::const_iterator lit = list.cbegin(); lit != list.cend(); ++lit)
	{
		UserDB::GetInstance().SendMessageToUser(message, *lit);
	}
}

bool	ChannelDB::SetMaxUsersInChannel(int channelId,
										unsigned int limit)
{
	DB::iterator	it = mDataBase.find(channelId);

	if (it == mDataBase.end())
	{
		return (false);
	}
	bool	isValueSetSuccessful = it->second.SetMaxActiveUsers(limit);
	return (isValueSetSuccessful);
}

unsigned int	ChannelDB::GetMaxUsersInChannel(int channelId) const
{
	DB::const_iterator	it = mDataBase.find(channelId);

	if (it == mDataBase.end())
	{
		return (-1);
	}
	return (it->second.GetMaxActiveUsers());
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
