#include "SparseBlockManager.h"

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/segment.hpp>

#include <boost/geometry/index/rtree.hpp>

// to store queries results
#include <vector>
#include <list>
#include <queue>
#include <algorithm>

// just for output
#include <iostream>
#include <boost/foreach.hpp>

namespace Common
{
	SparseBlockManager::SparseBlockManager(Common::PngImage & image)
		: image(image)
	{}
	SparseBlockManager::~SparseBlockManager()
	{}

	bool SparseBlockManager::process()
	{
		bool success = true;

		success &= this->clearSparseBlocks();
		success &= this->makeSparseBlock();
		success &= this->mergeSparseBlock();
		success &= this->initImageToZero();
		success &= this->initImageWithSparseBlocks();

		return success;
	}

	bool SparseBlockManager::makeSparseBlock()
	{
		bool success = true;
		success &= this->clearSparseBlocks();

		int height{ static_cast<int>(image.getHeight()) };
		int width = image.getWidth();
		bool** isConquered = new bool*[height];
		int leftest = 0, rightest = 0, topist = 0, bottomest = 0, count;
		
		// all elements set to false
		for (int i = 0; i < height; i++)
			*(isConquered + i) = new bool[width]();

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				if (image[i][j].R == 0
					&& isConquered[i][j] == false)
				{
					int top = i, bottom = i, left = j, right = j;
					count = 0;
					std::queue<std::pair<int, int>> q;
					q.emplace(i, j);
					isConquered[i][j] = true;

					while (!q.empty())
					{
						auto elem = q.front();
						q.pop();

						++count;

						if (elem.first < top)
							top = elem.first;
						else if (bottom < elem.first)
							bottom = elem.first;
						if (elem.second < left)
							left = elem.second;
						else if (right < elem.second)
							right = elem.second;

						for (int i = 0; i < directionsLength; i++)
						{
							int ty = elem.first + directions[i][0];
							int tx = elem.second + directions[i][1];
							if (ty < 0 || tx < 0 || ty >= height || tx >= width)
								continue;
							if (isConquered[ty][tx] == true || image[ty][tx].R != 0)
								continue;
							isConquered[ty][tx] = true;
							q.emplace(ty, tx);
						}
					}

					this->sparseBlocks.emplace_back(point(left, top), point(right, bottom), (right - left) * (-top + bottom));
					if (left < leftest)
						leftest = left;
					if (right > rightest)
						rightest = right;
					if (top < topist)
						topist = top;
					if (bottom > bottomest)
						bottomest = bottom;
				}
			}
		}

		for (int i = 0; i < height; i++)
			delete [] *(isConquered + i);
		delete [] isConquered;

		success &= this->arrangeSparseBlocks();

		this->letterHeightAvg = this->letterWidthAvg = 0;
		for (auto& p : this->sparseBlocks)
		{
			letterHeightAvg += -p.min_corner().get<1>() + p.max_corner().get<1>();
			letterWidthAvg += -p.min_corner().get<0>() + p.max_corner().get<0>();
		}
		letterHeightAvg /= this->sparseBlocks.size();
		letterWidthAvg /= this->sparseBlocks.size();

		return success;
	}
	bool SparseBlockManager::mergeSparseBlock()
	{
		std::vector<box> result_n;
		const double STANDARD_VALUE = static_cast<double>(static_cast<int>(this->getLetterWidthAvg() / 2 + 0.5));
		bool isDeleted = true;

		for (auto& p : sparseBlocks)
		{
			rtree.insert(p);
		}

		while (isDeleted)
		{
			isDeleted = false;

			for (auto itr = std::begin(sparseBlocks); itr != std::end(sparseBlocks); )
			{
				result_n.clear();
				rtree.query(bgi::nearest(static_cast<box&>(*itr), 2), std::back_inserter(result_n));
				double dist = 987654321; // for trace
				int deletedIndex = -1, k = 0;
				for (auto p : result_n)
				{
					if (itr->max_corner().get<0>() != p.max_corner().get<0>()
						|| itr->max_corner().get<1>() != p.max_corner().get<1>()
						|| itr->min_corner().get<0>() != p.min_corner().get<0>()
						|| itr->min_corner().get<1>() != p.min_corner().get<1>())
					{
						dist = bg::distance(static_cast<box&>(*itr), p);
						deletedIndex = k;
						break;
					}
					++k;
				}
				if (dist != 987654321)
				{
					// for Debug...
					if (dist < STANDARD_VALUE)
					{
						auto left = std::min(itr->min_corner().get<0>(), result_n[deletedIndex].min_corner().get<0>());
						auto right = std::max(itr->max_corner().get<0>(), result_n[deletedIndex].max_corner().get<0>());
						auto top = std::min(itr->min_corner().get<1>(), result_n[deletedIndex].min_corner().get<1>());
						auto bottom = std::max(itr->max_corner().get<1>(), result_n[deletedIndex].max_corner().get<1>());

						this->sparseBlocks.emplace_back(point(left, top), point(right, bottom));

						auto p = std::find(std::begin(sparseBlocks), std::end(sparseBlocks), SparseBlock(result_n[deletedIndex]));
						sparseBlocks.erase(p);

						rtree.remove(static_cast<box&>(*itr));
						rtree.remove(result_n[deletedIndex]);
						rtree.insert(box(point(left, top), point(right, bottom)));

						itr = this->sparseBlocks.erase(itr);


						isDeleted = true;
					}
					else
						++itr;
				}
				else
					++itr;
			}
		}

		this->sparseBlockHeightAvg = this->sparseBlockWidthAvg = 0;
		for (auto& p : rtree)
		{
			sparseBlockHeightAvg += -p.min_corner().get<1>() + p.max_corner().get<1>();
			sparseBlockWidthAvg += -p.min_corner().get<0>() + p.max_corner().get<0>();
		}
		sparseBlockHeightAvg /= rtree.size();
		sparseBlockWidthAvg /= rtree.size();

		return true;
	}

	bool SparseBlockManager::clearSparseBlocks()
	{
		this->sparseBlocks.clear();
		return true;
	}

	bool SparseBlockManager::arrangeSparseBlocks()
	{
		const int BOUNDARY = 1;
		auto itr = std::begin(this->sparseBlocks);
		int area;
		int i, j;
		int maxYBoundary = this->image.getHeight();
		int maxXBoundary = this->image.getWidth();
		int dotCounter;

		int offsetX, offsetY, edgeX, edgeY;
		while (itr != std::end(this->sparseBlocks))
		{
			area = itr->getRealArea();
			if (area <= 8)
			{
				offsetX = itr->getLeft();
				offsetY = itr->getTop();
				edgeX = itr->getRight();
				edgeY = itr->getBottom();

				i = ((offsetY - BOUNDARY > 0) ? offsetY - BOUNDARY : 0);
				dotCounter = 0;
				for (; i < maxYBoundary && i <= edgeY + BOUNDARY; ++i)
				{
					j = ((offsetX - BOUNDARY > 0) ? offsetX - BOUNDARY : 0);

					for (; j < maxXBoundary && j <= edgeX + BOUNDARY; ++j)
					{
						if (offsetY <= i && i <= edgeY
							&& offsetX <= j && j <= edgeX)
							continue;
						if (image[i][j].R == 0)
							++dotCounter;
					}
				}

				if (dotCounter < 4)
				{
					itr = this->sparseBlocks.erase(itr);
					continue;
				}
				else
				{
					++itr;
				}
			}
			else
			{
				++itr;
			}
		}
		return true;
	}

	bool SparseBlockManager::initImageToZero()
	{
		bool success = false;
		int w = this->image.getWidth();
		int h = this->image.getHeight();

		for (int i = 0; i < h; ++i)
		{
			for (int j = 0; j < w; ++j)
			{
				(this->image)[i][j].B = (this->image)[i][j].G = (this->image)[i][j].R = 255;
			}
		}

		success = true;
		return success;
	}

	bool SparseBlockManager::initImageWithSparseBlocks()
	{
		bool success = false;
		int offsetX, offsetY, edgeX, edgeY;

		for (auto& q : sparseBlocks)
		{
			offsetX = q.min_corner().get<0>();
			offsetY = q.min_corner().get<1>();
			edgeX = q.max_corner().get<0>();
			edgeY = q.max_corner().get<1>();
			for (int i = offsetY; i <= edgeY; ++i)
			{
				for (int j = offsetX; j <= edgeX; j++)
				{
					(this->image)[i][j].B = (this->image)[i][j].G = (this->image)[i][j].R = 0;
				}
			}
		}

		success = true;
		return success;
	}

	bool SparseBlockManager::hasCollisionWithSparseBlock(int top, int bottom,
		int left, int right)
	{
		std::vector<box> result_n;
		rtree.query(bgi::intersects(box(point(left, top), point(right, bottom))), std::back_inserter(result_n));
		return !(result_n.size() == 0);
	}
}