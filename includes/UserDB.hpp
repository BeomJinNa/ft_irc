#pragma once
#ifndef USERDB_HPP
# define USERDB_HPP

# include <string>
# include <map>

class User;
class Channel;

class UserDB
{
	public:
		typedef std::map<int, User>			DB;
		typedef std::map<std::string, int>	RefDB;

		UserDB(void);
		~UserDB(void);

		void				DoNothing(void) const;
		void				ConnectUser(int clientFd); //Server에서 호출 전용

		void				RemoveUserData(int clientFd); //Server에서 호출 전용
		void				DisconnectUser(int clientFd);

		void				SetLoginStatus(int clientFd, bool value);
		bool				GetLoginStatus(int clientFd) const;

		void				SetUserName(int clientFd, const std::string& name);
		const std::string&	GetUserName(int clientFd) const;
		void				SetNickName(int clientFd, const std::string& name);
		const std::string&	GetNickName(int clientFd) const;
		bool				IsUserAuthorized(int clientFd) const;

		int					GetClientFdByUserName(const std::string& userName) const;
		int					GetClientFdByNickName(const std::string& nickName) const;

		static UserDB&		GetInstance(void);

	private:
		UserDB(const UserDB& source);
		UserDB&	operator=(const UserDB& source);

		DB		mDataBase;
		RefDB	mReferenceTableUserName;
		RefDB	mReferenceTableNickName;
};
/*
 * ConnectUser:				서버에서 UserDB로 요청하기 위한 메소드입니다.
 *							클라이언트 소켓 fd를 기준으로 새 유저 정보를 DB에
 *							추가합니다.
 *
 * RemoveUserData:			서버에서 UserDB로 요청하기 위한 메소드입니다.
 *							일반적으로 사용되지 않습니다.
 *
 * DisconnectUser:			RemoveUserData를 내부적으로 호출한 후 서버에 연결
 *							해제를 요청합니다.
 *
 * GetLoginStatus:			클라이언트가 연결 후 password 인증을 받았는지 여부를
 *							반환합니다. 존재하지 않는 clientFd를 입력할 경우
 *							false를 반환합니다.
 *
 * IsUserAuthorized:		클라이언트가 로그인 후 username, nickname 설정을
 *							마쳤는지 여부를 반환합니다.
 *
 * GetUserName:				존재하지 않는 clientFd를 입력할 경우
 *							std::out_of_range를 던집니다.
 *
 * GetNickName:				존재하지 않는 clientFd를 입력할 경우
 *							std::out_of_range를 던집니다.
 *
 * GetClientFdByUserName:	설정된 유저 이름을 기준으로 해당 유저의 clientFd를
 *							반환 합니다. 존재하지 않는 이름일 경우 -1을
 *							반환합니다.
 */
#endif
