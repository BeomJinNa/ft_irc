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
		typedef std::map<int, int>			SocketRefDB;
		typedef std::vector<int>			ChannelList;

		UserDB(void);
		~UserDB(void);

		void			DoNothing(void) const;
		void			DisconnectUser(int userId);

		bool			IsUserIdValid(int userId) const;

		bool			AddChannelInUserList(int userId, int channelId);
		void			RemoveChannelInUserList(int userId, int channelId);

		void			SetLoginStatus(int userId, bool value);
		bool			GetLoginStatus(int userId) const;
		bool			IsUserAuthorized(int userId) const;

		void			SetUserName(int userId, const std::string& name);
		std::string		GetUserName(int userId) const;
		void			SetNickName(int userId, const std::string& name);
		std::string		GetNickName(int userId) const;

		void			SetHostName(int userId, const std::string& name);
		std::string		GetHostName(int userId) const;
		void			SetHostAddress(int userId, const std::string& name);
		std::string		GetHostAddress(int userId) const;

		ChannelList		GetJoinnedChannelList(int userId) const;

		int				GetUserIdByUserName(const std::string& userName) const;
		int				GetUserIdByNickName(const std::string& nickName) const;
		int				GetUserIdBySocketId(int socketId) const;
		int				GetSocketIdByUserId(int userId) const;

		void			SendMessageToUser(const std::string& message,
										  int userId) const;
		void			SendFormattedMessageToUser(const std::string& message,
												   int userId,
												   int targetUserId) const;
		void			SendCodeMessageToUser(const std::string& message,
											   int userId, int code,
											   int targetUserId) const;
		void			SendErrorMessageToUser(const std::string& message,
											   int userId, int code,
											   int targetUserId) const;
		std::string		GetFormattedMessage(const std::string& message,
											int userId) const;
		std::string		GetErrorMessage(const std::string& message,
										int userId, int code) const;

		static UserDB&	GetInstance(void);


		bool			ConnectUser(int userId);
		int				RemoveUserData(int userId);


		void			WriteChannelInUserData(int userId, int channelId);
		void			RemoveChannelInAllUsers(int channelId);


	private:
		UserDB(const UserDB& source);
		UserDB&	operator=(const UserDB& source);

		DB				mDataBase;
		RefDB			mReferenceTableUserName;
		RefDB			mReferenceTableNickName;
		SocketRefDB		mReferenceTableSocket;
		IndexManager	mIndex;
};

#endif
