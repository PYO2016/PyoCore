#include "SparseBlockManager.h"

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>

#include <boost/geometry/index/rtree.hpp>

// to store queries results
#include <vector>
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
					sparseBlocks.emplace_back(point(left, top), point(right, bottom));
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
		return true;
	}
	bool SparseBlockManager::mergeSparseBlock()
	{
		std::vector<box> result_n;
		std::vector<box> result_i;

		bool isDeleted = true;

		while (isDeleted)
		{
			isDeleted = false;
			//for (auto it = rtree.begin(); it != rtree.end(); ++it)
			result_i.clear();
			rtree.query(bgi::covered_by(*(this->covered)), std::back_inserter(result_i));
			//for (auto it = rtree.qbegin(); it != rtree.qend(); ++it)
			for (int i = 0; i < result_i.size(); i++)
			{
				result_n.clear();
				rtree.query(bgi::nearest(result_i[i], 2), std::back_inserter(result_n));
				std::cout << "pause";
				double dist = 987654321; // for trace
				int deletedIndex = -1, k = 0;
				for (auto p : result_n)
				{
					if (result_i[i].max_corner().get<0>() != p.max_corner().get<0>()
						|| result_i[i].max_corner().get<1>() != p.max_corner().get<1>()
						|| result_i[i].min_corner().get<0>() != p.min_corner().get<0>()
						|| result_i[i].min_corner().get<1>() != p.min_corner().get<1>())
					{
						dist = bg::distance(result_i[i], p);
						deletedIndex = k;
						break;
					}
					++k;
				}
				if (dist != 987654321)
				{
					// for Debug...
					double MOOSNSU = 10;
					if (dist < MOOSNSU)
					{
						auto left = min(result_i[i].min_corner().get<0>(), result_n[deletedIndex].min_corner().get<0>());
						auto right = max(result_i[i].max_corner().get<0>(), result_n[deletedIndex].max_corner().get<0>());
						auto top = min(result_i[i].min_corner().get<1>(), result_n[deletedIndex].min_corner().get<1>());
						auto bottom = max(result_i[i].max_corner().get<1>(), result_n[deletedIndex].max_corner().get<1>());

						rtree.remove(result_i[i]);
						rtree.remove(result_n[deletedIndex]);
						rtree.insert(box(point(left, top), point(right, bottom)));

						isDeleted = true;
						break;
					}
				}
			}
		}
		return false;
	}

	std::vector<Common::box> SparseBlockManager::getSparseBlocks()
	{
		vector<Common::box> b;
		for (auto p : rtree)
		{
			b.push_back(p);
		}
		return b;
	}

	bool SparseBlockManager::clearSparseBlocks()
	{
		this->sparseBlocks.clear();
		return true;
	}
}