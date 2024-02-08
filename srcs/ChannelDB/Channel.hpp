#pragma once
#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <set>
# include <vector>

class Channel
{
	public:
		typedef std::set<int>	DB;

		Channel(void);
		Channel(int channelId, const std::string& channelName);
		Channel(const Channel& source);
		Channel&	operator=(const Channel& source);
		~Channel(void);

							//성공 여부 반환
		bool				SetMaxActiveUsers(unsigned int maxUsers);
		unsigned int		GetMaxActiveUsers(void) const;

		void				RemoveUserData(int userId);
							//성공 여부 반환
		bool				AddActiveUser(int userId);
		void				RemoveActiveUser(int userId);
		bool				IsUserActive(int userId) const;

							//성공 여부 반환
		bool				AddOperator(int userId);
		void				RemoveOperator(int userId);
		bool				IsUserOperator(int userId) const;

							//성공 여부 반환
		void				AddInvitedUser(int userId);
		void				RemoveInvitedUser(int userId);
		bool				IsUserInvited(int userId) const;

		void				AddBanUser(int userId);
		void				RemoveBanUser(int userId);
		bool				IsUserBanned(int userId) const;

		void				SetChannelId(int id);
		unsigned int		GetChannelId(void) const;

		void				SetChannelName(const std::string& name);
		std::string&		GetChannelName(void);
		const std::string&	GetChannelName(void) const;

		void				SetChannelMode(unsigned int mode);
		unsigned int		GetChannelMode(void) const;

		void				SetPassword(const std::string& password);
		std::string&		GetPassword(void);
		const std::string&	GetPassword(void) const;

		void				SetTopic(const std::string& topic);
		std::string&		GetTopic(void);
		const std::string&	GetTopic(void) const;

		std::vector<int>	GetActiveUserList(void) const;
		std::vector<int>	GetOperatorList(void) const;
		std::vector<int>	GetInvitedUserList(void) const;
		std::vector<int>	GetBanUserList(void) const;

	private:
		int				mChannelId;
		std::string		mName;
		unsigned int	mChannelMode;
		unsigned int	mMaxActiveUsers;
		unsigned int	mCurrentActiveUsers;
		std::string		mPassword;
		std::string		mTopic;

		DB				mActiveUserList;
		DB				mOperatorList;
		DB				mInvitedUserList;
		DB				mBanUserList;
};
#endif
