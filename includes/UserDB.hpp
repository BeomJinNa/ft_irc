#pragma once
#ifndef USERDB_HPP
# define USERDB_HPP

# include <string>
# include <map>
# include "IndexManager.hpp"

class User;
class Channel;

class UserDB
{
	public: typedef std::map<int, User>			DB;
		typedef std::map<std::string, int>	RefDB;

		UserDB(void);
		~UserDB(void);

		void				DoNothing(void) const;
		void				ConnectUser(int clientFd); //Server에서 호출 전용

		int					RemoveUserData(int clientFd); //Server에서 호출 전용
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

		DB				mDataBase;
		RefDB			mReferenceTableUserName;
		RefDB			mReferenceTableNickName;
		IndexManager	mIndex;
};
/*
 * 각 유저는 cliendFd를 키로 mDataBase 트리에서 관리됩니다. cliendFd는
 * 추상화된 유저 데이터 인덱스로 실제 소켓의 fd는 내부적으로 관리됩니다.
 * 실제 소켓의 포트 번호에 실수로 직접 접근하는 것을 방지하기 위해
 * 인덱스(clientFd)는 100000번 부터 차례대로 번호가 부여됩니다. 후킹 함수에서
 * 유저 연결을 끊을 경우 안전한 자원 회수를 위해서 close 함수를 직접 호출하는
 * 대신에 UserDB에 DisconnectUser 메소드를 통해 연결 해제를 요청해야 합니다.
 *
 * ConnectUser:				서버에서 UserDB로 요청하기 위한 메소드입니다.
 *							클라이언트 소켓 fd를 기준으로 새 유저 정보를 DB에
 *							추가합니다.
 *
 * RemoveUserData:			서버에서 UserDB로 요청하기 위한 메소드입니다.
 *							일반적으로 사용되지 않습니다. 소켓의 파일 디스크립터
 *							값을 반환합니다.
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
