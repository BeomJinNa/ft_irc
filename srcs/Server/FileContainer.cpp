#include <unistd.h>
#include "FileContainer.hpp"

FileContainer::FileContainer(void) : initSuccess(false) {}
FileContainer::FileContainer(const FileContainer& source)
	: mFdList(source.mFdList)
	, initSuccess(false) {}

FileContainer&	FileContainer::operator=(const FileContainer& source)
{
	if (this != &source)
	{
		mFdList = source.mFdList;
		initSuccess = source.initSuccess;
	}

	return (*this);
}

FileContainer::~FileContainer(void)
{
	if (initSuccess)
	{
		return ;
	}

	for (std::vector<int>::iterator	it = mFdList.begin(); it != mFdList.end(); ++it)
	{
		close(*it);
	}
}

void	FileContainer::AddFd(int fd)
{
	mFdList.push_back(fd);
}

void	FileContainer::SetSuccess(void)
{
	initSuccess = true;
}
