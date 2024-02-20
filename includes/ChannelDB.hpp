#pragma once
#ifndef CHANNELDB_HPP
# define CHANNELDB_HPP

# include <string>
# include <map>
# include <ctime>
# include "IndexManager.hpp"

class Channel;

class ChannelDB
{
	public:
		typedef std::map<int, Channel>		DB;
		typedef std::map<std::string, int>	RefDB;
		typedef std::vector<int>			UserList;

		ChannelDB(void);
		~ChannelDB(void);

		void			DoNothing(void) const;
		int				CreateChannel(const std::string& name);
		void			DeleteChannel(int channelId);

		bool			IsChannelIdValid(int channelId) const;
		bool			IsUserInChannel(int channelId, int userId) const;

		bool			AddUserIntoChannel(int channelId, int userId);
		void			RemoveUserFromChannel(int channelId, int userId);

		bool			AddOperatorIntoChannel(int channelId, int userId);
		void			RemoveOperatorFromChannel(int channelId, int userId);
		bool			IsUserOperator(int channelId, int userId) const;

		void			AddInvitedUserIntoChannel(int channelId, int userId);
		void			RemoveInvitedUserFromChannel(int channelId, int userId);
		bool			IsUserInvited(int channelId, int userId) const;

		int				GetChannelIdByName(const std::string& name) const;

		bool			SetMaxUsersInChannel(int channelId, unsigned int limit);
		unsigned int	GetMaxUsersInChannel(int channelId) const;
		unsigned int	GetCurrentUsersInChannel(int channelId) const;

		void			SetChannelName(int channelId,
										const std::string& name);
		std::string		GetChannelName(int channelId);

		void			SetChannelFlag(int channelId, unsigned int flag);
		unsigned int	GetChannelFlag(int channelId) const;

		void			AddChannelFlag(int channelId, unsigned int flag);
		void			RemoveChannelFlag(int channelId, unsigned int flag);

		void			SetChannelPassword(int channelId,
										   const std::string& password);
		std::string		GetChannelPassword(int channelId) const;

		void			SetChannelTopic(int channelId,
										const std::string& topic,
										int setUserId);
		std::string		GetChannelTopic(int channelId) const;
		int				GetChannelTopicSetUser(int channelId) const;

		std::time_t		GetCreatedTimeOfChannel(int channelId) const;
		std::string		GetCreatedTimeStampOfChannel(int channelId) const;

		UserList		GetUserListInChannel(int channelId) const;
		UserList		GetOperatorListInChannel(int channelId) const;
		UserList		GetBanListInChannel(int channelId) const;

		void			AnnounceToChannel(const std::string& message,
											 int channelId) const;
		void			AnnounceErrorToChannel(const std::string& message,
												  int channelId, int code) const;
		void			AnnounceFormattedToChannel(const std::string& message,
													  int channelId) const;
		void			AnnounceErrorToChannel(const std::string& message,
												  int channelId, int code,
												  int userId) const;
		void			AnnounceFormattedToChannel(const std::string& message,
													  int channelId,
													  int userId) const;

		static ChannelDB&	GetInstance(void);
		void			DeleteUserInAllChannels(int userId);

	private:
		ChannelDB(const ChannelDB& source);
		ChannelDB&	operator=(const ChannelDB& source);

		DB				mDataBase;
		IndexManager	mIndex;
		RefDB			mReferenceTableName;
};

#endif
