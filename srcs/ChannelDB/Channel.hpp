#pragma once
#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <set>
# include <vector>

class Channel
{
	public:
		Channel(void);
		Channel(const std::string& channelName);
		Channel(const Channel& source);
		Channel&	operator=(const Channel& source);
		~Channel(void);

		std::vector<int>	GetActiveUserList(void) const;
		std::vector<int>	GetOperatorList(void) const;
		std::vector<int>	GetBanUserList(void) const;

		void				SetChannelMode(unsigned int mode);
		unsigned int		GetChannelMode(void) const;

		void				SetMaxActiveUsers(unsigned int maxUsers);
		unsigned int		GetMaxActiveUsers(void) const;

		void				SetPassword(const std::string& password);
		std::string&		GetPassword(void);
		const std::string&	GetPassword(void) const;

		void				SetTopic(const std::string& topic);
		std::string&		GetTopic(void);
		const std::string&	GetTopic(void) const;

		void				AddActiveUser(int userId);
		void				RemoveActiveUser(int userId);

		void				AddOperator(int userId);
		void				RemoveOperator(int userId);

		void				AddBanUser(int userId);
		void				RemoveBanUser(int userId);

	private:
		std::string		mName;
		unsigned int	mChannelMode;
		unsigned int	mMaxActiveUsers;
		std::string		mPassword;
		std::string		mTopic;

		std::set<int>	mActiveUserList;
		std::set<int>	mOperatorList;
		std::set<int>	mBanUserList;
};

enum ChannelMode
{
	M_FLAG_CHANNEL_INVITE_ONLY			= 1 << 0,
	M_FLAG_CHANNEL_TOPIC_OPERATOR_ONLY	= 1 << 1,
};
#endif
