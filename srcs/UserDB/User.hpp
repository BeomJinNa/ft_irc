#pragma once
#ifndef USER_HPP
# define USER_HPP

# include <string>
# include <set>
# include <vector>

class Channel;

class User
{
	public:
		User(void);
		User(int userId, int socketFd);
		User(const User& source);
		User&	operator=(const User& source);
		~User(void);

		void				SetUserId(int value);
		int					GetUserId(void) const;

		void				SetSocketFd(int value);
		int					GetSocketFd(void) const;

		void				SetLoginStatus(bool value);
		bool				GetLoginStatus(void) const;

		void				SetUserName(const std::string& name);
		std::string&		GetUserName(void);
		const std::string&	GetUserName(void) const;

		void				SetNickName(const std::string& name);
		std::string&		GetNickName(void);
		const std::string&	GetNickName(void) const;

		void				SetHostName(const std::string& name);
		std::string&		GetHostName(void);
		const std::string&	GetHostName(void) const;

		void				SetHostAddress(const std::string& name);
		std::string&		GetHostAddress(void);
		const std::string&	GetHostAddress(void) const;

		void				SetFlagUserNameSet(bool value);
		bool				GetFlagUserNameSet(void) const;

		void				SetFlagNickNameSet(bool value);
		bool				GetFlagNickNameSet(void) const;

		std::vector<int>	GetChannelList(void) const;
		void				AddChannelInJoinnedList(int channelId);
		bool				RemoveChannelInJoinnedList(int channelId);

	private:
		int				mUserId;
		int				mSocketFd;
		bool			mIsLoggedIn;
		bool			mIsUserNameSet;
		bool			mIsNickNameSet;
		std::string		mUserName;
		std::string		mNickName;
		std::string		mHostName;
		std::string		mHostAddress;
		std::set<int>	mJoinedChannels;
};
#endif
