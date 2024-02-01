#include "CommandContainer.hpp"
#include "Command.hpp"

CommandContainer::CommandContainer(void)
	: mCommand(NULL)
{
	mCommandList.insert("PASS");
	mCommandList.insert("QUIT");
	mCommandList.insert("USER");
	mCommandList.insert("NICK");
//	mCommandList.insert("INVITE");
//	mCommandList.insert("JOIN");
//	mCommandList.insert("KICK");
//	mCommandList.insert("MODE");
//	mCommandList.insert("NOTICE");
//	mCommandList.insert("PART");
//	mCommandList.insert("PRIVMSG");
//	mCommandList.insert("TOPIC");
}

CommandContainer::CommandContainer(const CommandContainer& source)
	: mCommandList(source.mCommandList)
{
}

CommandContainer&	CommandContainer::operator=(const CommandContainer& source)
{
	if (this != &source)
	{
		mCommand = source.mCommand;
		mCommandList = source.mCommandList;
	}

	return (*this);
}

CommandContainer::~CommandContainer(void)
{
	delete mCommand;
}

bool	CommandContainer::SetCommand(const std::string& command)
{
	DB::iterator	it = mCommandList.find(command);

	delete mCommand;
	mCommand = NULL;

	if (it == mCommandList.end())
	{
		return (false);
	}
	return (true);
}

void	CommandContainer::CreateClassPASS(void)
{

}
