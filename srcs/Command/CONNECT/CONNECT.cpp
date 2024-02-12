#include <set>
#include <string>
#include <vector>
#include <sstream>
#include <utility>
#include "Server.hpp"
#include "UserDB.hpp"
#include "ChannelDB.hpp"
#include "ChannelMode.hpp"
#include "Message.hpp"
#include "ErrorCodes.hpp"
#include "ReplyCodes.hpp"

/*

	CONNECT <target server> [<port> [<remote server>]]

    ERR_NOSUCHSERVER (402)
    ERR_NEEDMOREPARAMS (461)
    ERR_NOPRIVILEGES (481)
    ERR_NOPRIVS (723)

*/

void	HookFunctionConnect(const Message& message)
{
}
