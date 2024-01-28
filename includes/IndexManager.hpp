#pragma once
#ifndef INDEXMANAGER_HPP
# define INDEXMANAGER_HPP

# include <set>

class IndexManager
{
	public:
		IndexManager(void);
		IndexManager(int min);
		IndexManager(int min, int max);
		IndexManager(const IndexManager& source);
		IndexManager&	operator=(const IndexManager& source);
		~IndexManager(void);

		int		GetNewIndex(void);
		void	DeactivateIndex(int index);

	private:
		int				mIndexMin;
		int				mIndexMax;
		int				mCurrentEndIndex;
		std::set<int>	mDeactivatedIndexes;
};
#endif
