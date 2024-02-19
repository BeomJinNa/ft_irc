#pragma once
#ifndef FILECONTAINER_HPP
# define FILECONTAINER_HPP

# include <vector>

class FileContainer
{
	public:
		FileContainer(void);
		FileContainer(const FileContainer& source);
		FileContainer&	operator=(const FileContainer& source);
		~FileContainer(void);

		void	AddFd(int fd);
		void	SetSuccess(void);
	private:
		std::vector<int>	mFdList;
		bool				initSuccess;
};
#endif
