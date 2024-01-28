#pragma once
#ifndef CHANNELDB_HPP
# define CHANNELDB_HPP

# include <map>
# include "IndexManager.hpp"

class Channel;

class ChannelDB
{
	public:
		typedef std::map<int, Channel>		DB;
		typedef std::map<std::string, int>	RefDB;

		ChannelDB(void);
		~ChannelDB(void);

		void			DoNothing(void) const;
		int				CreateChannel(const std::string& name);
		void			DeleteChannel(int channelId);

		bool			AddUserInChannel(int channelId, int userId);
		void			RemoveUserInChannel(int channelId, int userId);

						//UserDB에서 호출 전용
		void			DeleteUserInAllChannels(int userId);

		int				GetChannelIdByName(const std::string& name) const;

		void			SetChannelFlag(int channelId, unsigned int flag);
		unsigned int	GetChannelFlag(int channelId) const;

		static ChannelDB&	GetInstance(void);
	private:
		ChannelDB(const ChannelDB& source);
		ChannelDB&	operator=(const ChannelDB& source);

		DB				mDataBase;
		IndexManager	mIndex;
		RefDB			mReferenceTableName;
};
/*
 * AddUserInChannel:	해당 채널에 해당하는 id의 유저를 입장시킵니다.
 *						실패시 false를 반환하며 채널에 이미 입장되어 있는 경우에
 *						true를 반환합니다.
 *
 * CreateChannel:		채널 생성 성공시 채널의 id를 반환합니다.
 *						실패시 -1 을 반환합니다.
 *
 * GetChannelIdByName:	해당 이름을 가진 채널의 id값을 받아옵니다.
 *						존재하지 않을 경우에는 -1 을 반환합니다.
 *
 * SetChannelFlag:		채널의 플래그 값을 입력합니다. 채널 모드 설정
 *						등에 활용 가능합니다.
 *
 * GetChannelFlag:		채널의 현재 플래그 값을 읽어옵니다. 존재하지 않는 채널의
 *						id 값을 인자로 준 경우 M_FLAG_CHANNEL_DOES_NOT_EXIST
 *						플래그를 반환합니다.(ChannelMode.hpp에 정의)
 */
#endif
