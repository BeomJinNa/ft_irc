#pragma once
#ifndef CHANNELDB_HPP
# define CHANNELDB_HPP

# include <string>
# include <map>
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

		bool			AddUserIntoChannel(int channelId, int userId);
		void			RemoveUserIntoChannel(int channelId, int userId);

		bool			AddOperatorIntoChannel(int channelId, int userId);
		void			RemoveOperatorIntoChannel(int channelId, int userId);
		bool			IsUserOperator(int channelId, int userId) const;

		void			AddBanIntoChannel(int channelId, int userId);
		void			RemoveBanIntoChannel(int channelId, int userId);
		bool			IsUserBanned(int channelId, int userId) const;

		int				GetChannelIdByName(const std::string& name) const;

		bool			SetMaxUsersInChannel(int channelId, unsigned int limit);
		unsigned int	GetMaxUsersInChannel(int channelId) const;

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
										const std::string& topic);
		std::string		GetChannelTopic(int channelId) const;

		UserList		GetUserListInChannel(int channelId) const;
		UserList		GetOperatorListInChannel(int channelId) const;
		UserList		GetBanListInChannel(int channelId) const;

		void			SendMessageToChannel(const std::string& message,
											 int channelId) const;

		static ChannelDB&	GetInstance(void);

		//UserDB에서 호출 전용
		void			DeleteUserInAllChannels(int userId);

	private:
		ChannelDB(const ChannelDB& source);
		ChannelDB&	operator=(const ChannelDB& source);

		DB				mDataBase;
		IndexManager	mIndex;
		RefDB			mReferenceTableName;
};
/*
 * CreateChannel:	채널 생성 성공시 채널의 id를 반환합니다.
 *                  실패시 -1을 반환합니다.
 *
 * AddUserIntoChannel: 해당 채널에 특정 유저를 추가합니다.
 *                     유저가 이미 채널에 존재하거나 추방된 상태가 아니면 true 반환.
 *                     채널/유저가 존재하지 않거나 유저가 추방된 경우 false 반환.
 *
 * RemoveUserIntoChannel: 특정 채널에서 유저를 제거합니다.
 *                        유저가 채널에 존재하지 않거나 채널이 없으면 아무 작업도 않음.
 *
 * AddOperatorIntoChannel: 특정 채널에서 유저를 운영자로 추가합니다.
 *                         유저가 추방되지 않은 경우 운영자로 추가하고 true 반환.
 *                         채널/유저가 없거나 유저가 추방된 경우 false 반환.
 *
 * RemoveOperatorIntoChannel: 채널에서 유저를 운영자 목록에서 제거합니다.
 *                            유저가 운영자 목록에 없거나 채널이 없으면 아무 작업도 않음.
 *
 * IsUserOperator: 특정 채널에서 유저가 운영자인지 확인합니다.
 *                 유저가 운영자 목록에 있으면 true, 아니면 false 반환.
 *                 채널이 없는 경우도 false 반환.
 *
 * AddBanIntoChannel: 특정 채널에서 유저를 추방 목록에 추가합니다.
 *                    유저는 추방 후 채널 입장 불가.
 *                    채널/유저가 없는 경우 아무 작업도 않음.
 *
 * RemoveBanIntoChannel: 채널에서 유저를 추방 목록에서 제거합니다.
 *                       유저가 추방 목록에 없거나 채널이 없으면 아무 작업도 않음.
 *
 * IsUserBanned: 특정 채널에서 유저가 추방된 상태인지 확인합니다.
 *               유저가 추방 목록에 있으면 true, 아니면 false 반환.
 *               채널이 없는 경우도 false 반환.
 *
 * DeleteUserInAllChannels: 특정 유저를 모든 채널에서 제거합니다.
 *                          유저 삭제 시 호출. 모든 채널에서 해당 유저의 데이터 삭제.
 *
 * GetChannelIdByName: 해당 이름의 채널 id를 받아옵니다.
 *                     존재하지 않을 경우 -1을 반환합니다.
 *
 * SetChannelFlag: 채널의 플래그 값을 설정합니다. 채널 모드 설정 등에 활용.
 *
 * GetChannelFlag: 채널의 현재 플래그 값을 읽어옵니다.
 *                 존재하지 않는 채널의 id값을 인자로 주면 M_FLAG_CHANNEL_DOES_NOT_EXIST
 *                 플래그 반환. (ChannelMode.hpp에 정의)
 */
#endif
