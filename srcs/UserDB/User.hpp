#pragma once
#ifndef USER_HPP
# define USER_HPP

# include <string>
# include <vector>

class Channel;

class User
{
	public:
		User(void);
		User(int clientFd);
		User(const User& source);
		User&	operator=(const User& source);
		~User(void);

		void				SetClientFd(int value);
		int					GetClientFd(void) const;

		void				SetLoginStatus(bool value);
		bool				GetLoginStatus(void) const;

		void				SetUserName(const std::string& name);
		std::string&		GetUserName(void);
		const std::string&	GetUserName(void) const;

		void				SetNickName(const std::string& name);
		std::string&		GetNickName(void);
		const std::string&	GetNickName(void) const;

		void				SetFlagUserNameSet(bool value);
		bool				GetFlagUserNameSet(void) const;

		void				SetFlagNickNameSet(bool value);
		bool				GetFlagNickNameSet(void) const;

	private:
		int						mClientFd;
		bool					mIsLoggedIn;
		bool					mIsUserNameSet;
		bool					mIsNickNameSet;
		std::string				mUserName;
		std::string				mNickName;
		std::vector<Channel*>	mJoinedChannels;
};
#endif
