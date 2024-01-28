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
	public:
		typedef std::map<int, User>			DB;
		typedef std::map<std::string, int>	RefDB;

		UserDB(void);
		~UserDB(void);

		void				DoNothing(void) const;
		void				DisconnectUser(int userId);

		bool				AddChannelInUserList(int userId, int channelId);
		void				RemoveChannelInUserList(int userId, int channelId);

		void				SetLoginStatus(int userId, bool value);
		bool				GetLoginStatus(int userId) const;

		void				SetUserName(int userId, const std::string& name);
		const std::string&	GetUserName(int userId) const;
		void				SetNickName(int userId, const std::string& name);
		const std::string&	GetNickName(int userId) const;
		bool				IsUserAuthorized(int userId) const;

		int					GetUserIdByUserName(const std::string& userName) const;
		int					GetUserIdByNickName(const std::string& nickName) const;

		static UserDB&		GetInstance(void);

							//Server에서 호출 전용
		bool				ConnectUser(int userId);
		int					RemoveUserData(int userId);

							//ChannelDB에서 호출 전용
		void				WriteChannelInUserData(int userId, int channelId);
		void				RemoveChannelInAllUsers(int channelId);


	private:
		UserDB(const UserDB& source);
		UserDB&	operator=(const UserDB& source);

		DB				mDataBase;
		RefDB			mReferenceTableUserName;
		RefDB			mReferenceTableNickName;
		IndexManager	mIndex;
};

/*
 * 각 유저는 userId를 인덱스 키로 mDataBase 트리에서 관리됩니다.
 * 인덱스(userId)는 100000번 부터 차례대로 번호가 부여됩니다.
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
 *							반환합니다. 존재하지 않는 userId를 입력할 경우
 *							false를 반환합니다.
 *
 * IsUserAuthorized:		클라이언트가 로그인 후 username, nickname 설정을
 *							마쳤는지 여부를 반환합니다.
 *
 * GetUserName:				존재하지 않는 userId를 입력할 경우
 *							std::out_of_range를 던집니다.
 *
 * GetNickName:				존재하지 않는 userId를 입력할 경우
 *							std::out_of_range를 던집니다.
 *
 * GetClientFdByUserName:	설정된 유저 이름을 기준으로 해당 유저의 userId를
 *							반환 합니다. 존재하지 않는 이름일 경우 -1을
 *							반환합니다.
 */
#endif
