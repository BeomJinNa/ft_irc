#include <ctime>
#include <string>
#include "UserDB.hpp"
#include "ChannelDB.hpp"
#include "Message.hpp"
#include "ErrorCodes.hpp"
#include "ReplyCodes.hpp"

//:yourserver.example.com CAP * LS :multi-prefix away-notify extended-join chghost
void	HookFunctionCap(const Message& message)
{
	UserDB&			userDB = UserDB::GetInstance();
	int				userId = message.GetUserId();

	userDB.SendMessageToUser(":localhost CAP * LS :multi-prefix away-notify extended-join chghost", userId);
}
