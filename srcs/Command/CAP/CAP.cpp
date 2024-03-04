#include <string>
#include "Message.hpp"
#include "UserDB.hpp"

void	HookFunctionCap(const Message& message)
{
	UserDB&			userDB = UserDB::GetInstance();
	int				userId = message.GetUserId();

	userDB.SendMessageToUser(":localhost CAP * LS :None", userId);
}
