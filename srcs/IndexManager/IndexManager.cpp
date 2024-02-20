#include "IndexManager.hpp"

namespace
{
	const int MAX_VALUE = std::numeric_limits<int>::max() - 1;
}

IndexManager::IndexManager(void)
	: mIndexMin(0)
	, mIndexMax(MAX_VALUE)
	, mCurrentEndIndex(0)
{
}

IndexManager::IndexManager(int min)
{
	if (min > MAX_VALUE)
	{
		mIndexMin = 0;
		mIndexMax = MAX_VALUE;
		mCurrentEndIndex = mIndexMin;
		return ;
	}
	if (min < 0)
	{
		min = 0;
	}
	mIndexMin = min;
	mIndexMax = MAX_VALUE;
	mCurrentEndIndex = mIndexMin;
}

IndexManager::IndexManager(int min, int max)
{
	if (min > max)
	{
		mIndexMin = 0;
		mIndexMax = MAX_VALUE;
		mCurrentEndIndex = mIndexMin;
		return ;
	}
	if (min < 0)
	{
		min = 0;
	}
	if (max > MAX_VALUE)
	{
		max = MAX_VALUE;
	}
	mIndexMin = min;
	mIndexMax = max;
	mCurrentEndIndex = mIndexMin;
}

IndexManager::IndexManager(const IndexManager& source)
	: mIndexMin(source.mIndexMin)
	, mIndexMax(source.mIndexMax)
	, mCurrentEndIndex(source.mCurrentEndIndex)
	, mDeactivatedIndexes(source.mDeactivatedIndexes)
{
}

IndexManager&	IndexManager::operator=(const IndexManager& source)
{
	if (this != &source)
	{
		mIndexMin			= source.mIndexMin;
		mIndexMax			= source.mIndexMax;
		mCurrentEndIndex	= source.mCurrentEndIndex;
		mDeactivatedIndexes	= source.mDeactivatedIndexes;
	}

	return (*this);
}

IndexManager::~IndexManager(void) {}

int	IndexManager::GetNewIndex(void)
{
	int	output;

	if (mDeactivatedIndexes.empty())
	{
		if (mCurrentEndIndex <= mIndexMax)
		{
			output = mCurrentEndIndex++;
			return (output);
		}
		return (-1);
	}

	const std::set<int>::iterator it = mDeactivatedIndexes.begin();
	output = *it;
	mDeactivatedIndexes.erase(it);
	return (output);
}

void	IndexManager::DeactivateIndex(int index)
{
	if (index < mIndexMin || index >= mCurrentEndIndex)
	{
		return ;
	}
	mDeactivatedIndexes.insert(index);
}
