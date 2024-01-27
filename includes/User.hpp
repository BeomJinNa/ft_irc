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
/*
 * std::map의 사용 조건을 충족하기 위해 복사 생성자와 복사 대입 연산자가 구현되어
 * 있습니다. 특별한 목적이 아니라면 User 객체의 복사 생성자 혹은 복사 대입 연산자
 * 사용은 하지 않는 것이 좋습니다.
 */
#endif
