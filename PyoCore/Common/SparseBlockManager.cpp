#include "SparseBlockManager.h"

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>

#include <boost/geometry/index/rtree.hpp>

// to store queries results
#include <vector>
#include <list>
#include <queue>

// just for output
#include <iostream>
#include <boost/foreach.hpp>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

typedef bg::model::point<int, 2, bg::cs::cartesian> point;
typedef bg::model::box<point> box;

namespace Common
{
	SparseBlockManager::SparseBlockManager(Common::PngImage & image)
		: image(image)
	{}
	SparseBlockManager::~SparseBlockManager()
	{
		if (this->covered != nullptr)
			delete this->covered;
	}
	bool SparseBlockManager::makeSparseBlock()
	{
		this->clearSparseBlocks();
		// queue for bfs
		int height{ static_cast<int>(image.getHeight()) };
		int width = image.getWidth();
		bool** isConquered = new bool*[height];
		int leftest = 0, rightest = 0, topist = 0, bottomest = 0;
		
		// all elements set to false
		for (int i = 0; i < height; i++)
			*(isConquered + i) = new bool[width]();

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				if (image[i][j].R == 0 /* valid value */
					&& isConquered[i][j] == false)
				{
					int top = i, bottom = i, left = j, right = j;
					std::queue<std::pair<int, int>> q;
					q.emplace(i, j);
					isConquered[i][j] = true;

					while (!q.empty())
					{
						auto elem = q.front();
						q.pop();

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
					rtree.insert(box(point(left, top), point(right, bottom)));
					this->sparseBlocks.emplace_back(point(left, top), point(right, bottom));
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
		covered = new box(point(leftest, topist), point(rightest, bottomest));

		initWidthHeight();
		return true;
	}
	bool SparseBlockManager::mergeSparseBlock()
	{
		std::vector<box> result_n;
		double MOOSNSU = getHeightAvg() / 2;

		bool isDeleted = true;

		while (isDeleted)
		{
			isDeleted = false;

			//for (int i = 0; i < result_i.size(); i++)
			for (auto itr = begin(sparseBlocks); itr != end(sparseBlocks); )
			{
				result_n.clear();
				rtree.query(bgi::nearest(*itr, 2), std::back_inserter(result_n));
				std::cout << "pause";
				double dist = 987654321; // for trace
				int deletedIndex = -1, k = 0;
				for (auto p : result_n)
				{
					if (itr->max_corner().get<0>() != p.max_corner().get<0>()
						|| itr->max_corner().get<1>() != p.max_corner().get<1>()
						|| itr->min_corner().get<0>() != p.min_corner().get<0>()
						|| itr->min_corner().get<1>() != p.min_corner().get<1>())
					{
						dist = bg::distance(*itr, p);
						deletedIndex = k;
						break;
					}
					++k;
				}
				if (dist != 987654321)
				{
					// for Debug...
					if (dist < MOOSNSU)
					{
						auto left = min(itr->min_corner().get<0>(), result_n[deletedIndex].min_corner().get<0>());
						auto right = max(itr->max_corner().get<0>(), result_n[deletedIndex].max_corner().get<0>());
						auto top = min(itr->min_corner().get<1>(), result_n[deletedIndex].min_corner().get<1>());
						auto bottom = max(itr->max_corner().get<1>(), result_n[deletedIndex].max_corner().get<1>());

						rtree.remove(*itr);
						rtree.remove(result_n[deletedIndex]);
						rtree.insert(box(point(left, top), point(right, bottom)));

						this->sparseBlocks.emplace_back(point(left, top), point(right, bottom));
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
		return true;
	}

	bool SparseBlockManager::clearSparseBlocks()
	{
		this->sparseBlocks.clear();
		return true;
	}

	bool SparseBlockManager::initWidthHeight()
	{
		heightAvg = widthAvg = 0;
		for (auto& p : rtree)
		{
			heightAvg += -p.min_corner().get<1>() + p.max_corner().get<1>();
			widthAvg += -p.min_corner().get<0>() + p.max_corner().get<0>();
		}
		heightAvg /= rtree.size();
		widthAvg /= rtree.size();
		return true;
	}
}