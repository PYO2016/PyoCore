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

#include "../TableDetection/HistogramManager.h"

namespace Common
{
	SparseBlockManager::SparseBlockManager(Common::PngImage & image)
		: image(image)
	{}
	SparseBlockManager::~SparseBlockManager()
	{}

	typedef bg::model::segment<point> segment;

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
		auto minWidth = INT_MAX;
		for (auto& p : this->sparseBlocks)
		{
			letterHeightAvg += -p.min_corner().get<1>() + p.max_corner().get<1>();
			letterWidthAvg += -p.min_corner().get<0>() + p.max_corner().get<0>();
			if (-p.min_corner().get<0>() + p.max_corner().get<0>() < minWidth)
			{
				minWidth = -p.min_corner().get<0>() + p.max_corner().get<0>();
			}
		}
		this->letterWidthMin = minWidth;
		letterHeightAvg /= this->sparseBlocks.size();
		letterWidthAvg /= this->sparseBlocks.size();

		return success;
	}
	bool SparseBlockManager::mergeSparseBlock()
	{
		std::vector<box> result_n;
		const double STANDARD_VALUE = this->getLetterWidthAvg() / 2;
		bool isDeleted = true;
		int left, right, top, bottom;
		double dist = -1; // for trace

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

				dist = -1; // for trace
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
				if (dist != -1 && dist < STANDARD_VALUE)
				{
					double rate = 3;
					// need height
					if (itr->min_corner().get<1>() < result_n[deletedIndex].max_corner().get<1>()
						|| result_n[deletedIndex].min_corner().get<1>() < itr->max_corner().get<1>())
						rate = static_cast<double>(itr->getHeight()) / (static_cast<double>(result_n[deletedIndex].max_corner().get<1>()) - static_cast<double>(result_n[deletedIndex].min_corner().get<1>()) + 1);
					// else need width
					else if (itr->min_corner().get<0>() < result_n[deletedIndex].max_corner().get<0>()
						|| result_n[deletedIndex].min_corner().get<0>() < itr->max_corner().get<0>())
						rate = static_cast<double>(itr->getWidth()) / (static_cast<double>(result_n[deletedIndex].max_corner().get<0>()) - static_cast<double>(result_n[deletedIndex].min_corner().get<0>()) + 1);
					if (rate > 5.0 || rate < 0.2)
					{
						// cant merge
						++itr;
						continue;
					}
					left = std::min(itr->min_corner().get<0>(), result_n[deletedIndex].min_corner().get<0>());
					right = std::max(itr->max_corner().get<0>(), result_n[deletedIndex].max_corner().get<0>());
					top = std::min(itr->min_corner().get<1>(), result_n[deletedIndex].min_corner().get<1>());
					bottom = std::max(itr->max_corner().get<1>(), result_n[deletedIndex].max_corner().get<1>());

					this->sparseBlocks.emplace_back(point(left, top), point(right, bottom));

					sparseBlocks.erase(std::find(std::begin(sparseBlocks), std::end(sparseBlocks), SparseBlock(result_n[deletedIndex])));
					rtree.remove(static_cast<box&>(*itr));
					rtree.remove(result_n[deletedIndex]);
					isDeleted = true;
					rtree.insert(box(point(left, top), point(right, bottom)));

					itr = this->sparseBlocks.erase(itr);

					isDeleted = true;
				}
				else
					++itr;
			}
		}

		// merge unsparsed cells
		isDeleted = true;
		while (isDeleted)
		{
			isDeleted = false;
			for (auto itr = std::begin(sparseBlocks); itr != std::end(sparseBlocks); )
			{
				result_n.clear();
				rtree.query(bgi::intersects(static_cast<box&>(*itr)), std::back_inserter(result_n));

				if (result_n.size() > 1)
				{
					left = top = INT_MAX;
					right = bottom = INT_MIN;
					isDeleted = true;
					for (auto& a : result_n)
					{
						left = std::min(left, a.min_corner().get<0>());
						right = std::max(right, a.max_corner().get<0>());
						top = std::min(top, a.min_corner().get<1>());
						bottom = std::max(bottom, a.max_corner().get<1>());

						for (auto jtr = std::begin(sparseBlocks); jtr != std::end(sparseBlocks); ++jtr)
							if (a.max_corner().get<1>() == jtr->max_corner().get<1>() && a.min_corner().get<0>() == jtr->min_corner().get<0>()
								&& a.min_corner().get<1>() == jtr->min_corner().get<1>() && a.max_corner().get<0>() == jtr->max_corner().get<0>())
							{
								sparseBlocks.erase(jtr);
								break;
							}
						rtree.remove(static_cast<box&>(a));
					}
					rtree.insert(box(point(left, top), point(right, bottom)));
					this->sparseBlocks.emplace_back(point(left, top), point(right, bottom));
					itr = std::begin(sparseBlocks);
				}
				else
					++itr;
			}
		}

		std::list<box> l;
		result_n.clear();
		for (const auto& p : rtree)
		{
			result_n.push_back(p);
		}
		std::sort(begin(result_n), end(result_n), [](const box& a, const box& b)
		{
			if (a.min_corner().get<1>() == b.min_corner().get<1>())
				return a.min_corner().get<0>() < b.min_corner().get<0>();
			return a.min_corner().get<1>() < b.min_corner().get<1>();
		});

		for (const auto& p : result_n)
		{
			l.push_back(p);
		}
		double kmeansboundary = 0;
		int addedN = 0;
		std::vector<int> v;

		for (auto itr = std::begin(l); itr != std::end(l);)
		{
			result_n.clear();
			auto currentBox = box(point(0, itr->min_corner().get<1>()), point(image.getWidth(), itr->max_corner().get<1>()));
			rtree.query(bgi::intersects(currentBox), std::back_inserter(result_n));
			std::sort(begin(result_n), end(result_n), [](const box& a, const box& b)
			{
				return a.min_corner().get<0>() < b.min_corner().get<0>();
			});
			for (int i = 0; i < result_n.size() - 1; )
			{
				double rate = 3;
				// need height
				if (itr->min_corner().get<1>() < result_n[i].max_corner().get<1>()
					|| result_n[i].min_corner().get<1>() < itr->max_corner().get<1>())
					rate = (static_cast<double>(itr->max_corner().get<1>()) - static_cast<double>(itr->min_corner().get<1>()))
						/ (static_cast<double>(result_n[i].max_corner().get<1>()) - static_cast<double>(result_n[i].min_corner().get<1>()) + 1);
				// else need width
				else if (itr->min_corner().get<0>() < result_n[i].max_corner().get<0>()
					|| result_n[i].min_corner().get<0>() < itr->max_corner().get<0>())
					rate = (static_cast<double>(itr->max_corner().get<0>()) - static_cast<double>(itr->min_corner().get<0>()))
						/ (static_cast<double>(result_n[i].max_corner().get<0>()) - static_cast<double>(result_n[i].min_corner().get<0>()) + 1);
				if (rate > 5.0 || rate < 0.2)
				{
					// cant merge
					result_n.erase(std::begin(result_n) + i);
					// ++itr;
					continue;
				}
				else
					++i;
			}
			for (int i = 0; i < result_n.size() - 1; ++i)
			{
				if (result_n[i + 1].min_corner().get<0>() - result_n[i].max_corner().get<0>() - 1 > 0)
				{
					v.emplace_back(result_n[i + 1].min_corner().get<0>() - result_n[i].max_corner().get<0>() - 1);
				}
			}

			for (const auto& a : result_n)
			{
				for (auto jtr = std::begin(l); jtr != std::end(l); ++jtr)
					if (a.max_corner().get<1>() == jtr->max_corner().get<1>() && a.min_corner().get<0>() == jtr->min_corner().get<0>()
						&& a.min_corner().get<1>() == jtr->min_corner().get<1>() && a.max_corner().get<0>() == jtr->max_corner().get<0>())
					{
						l.erase(jtr);
						break;
					}
					
			}
			itr = std::begin(l);
		}
		//Common::EMCluster em;
		//em.setArgs(v);
		//em.process();
		double ret1 = 12341234;// em.getMaxInMinClusterVal();
		double ret2 = getKmeansBoundary(v);
		//kmeansboundary = std::min(ret1, ret2);
		//kmeansboundary = 12;
		kmeansboundary = getKmeansBoundary(v);



		result_n.clear();
		l.clear();
		for (const auto& p : rtree)
			result_n.push_back(p);

		std::sort(begin(result_n), end(result_n), [](const box& a, const box& b)
		{
			if (a.min_corner().get<1>() == b.min_corner().get<1>())
				return a.min_corner().get<0>() < b.min_corner().get<0>();
			return a.min_corner().get<1>() < b.min_corner().get<1>();
		});

		for (const auto& p : result_n)
			l.push_back(p);

		isDeleted = true;
		while (isDeleted)
		{
			isDeleted = false;
			for (auto itr = std::begin(l); itr != std::end(l);)
			{
				result_n.clear();
				auto currentBox = box(point(0, itr->min_corner().get<1>()), point(image.getWidth(), itr->max_corner().get<1>()));
				rtree.query(bgi::intersects(currentBox), std::back_inserter(result_n));
				std::sort(begin(result_n), end(result_n), [](const box& a, const box& b)
				{
					return a.min_corner().get<0>() < b.min_corner().get<0>();
				});
				for (int i = 0; i < result_n.size() - 1; )
				{
					double rate = 3;
					// need height
					if (itr->min_corner().get<1>() < result_n[i].max_corner().get<1>()
						|| result_n[i].min_corner().get<1>() < itr->max_corner().get<1>())
						rate = (static_cast<double>(itr->max_corner().get<1>()) - static_cast<double>(itr->min_corner().get<1>()))
						/ (static_cast<double>(result_n[i].max_corner().get<1>()) - static_cast<double>(result_n[i].min_corner().get<1>()) + 1);
					// else need width
					else if (itr->min_corner().get<0>() < result_n[i].max_corner().get<0>()
						|| result_n[i].min_corner().get<0>() < itr->max_corner().get<0>())
						rate = (static_cast<double>(itr->max_corner().get<0>()) - static_cast<double>(itr->min_corner().get<0>()))
						/ (static_cast<double>(result_n[i].max_corner().get<0>()) - static_cast<double>(result_n[i].min_corner().get<0>()) + 1);
					if (rate > 5.0 || rate < 0.2)
					{
						// cant merge
						result_n.erase(std::begin(result_n) + i);
						// ++itr;
						continue;
					}
					else
						++i;
				}
				left = top = INT_MAX;
				right = bottom = INT_MIN;
				for (int i = 0; i < result_n.size() - 1; ++i)
				{
					dist = bg::distance(static_cast<box&>(result_n[i]), static_cast<box&>(result_n[i + 1]));
					if (dist >= kmeansboundary)
					{
						if (left != INT_MAX)
						{
							auto& a = result_n[i];
							left = std::min(left, a.min_corner().get<0>());
							right = std::max(right, a.max_corner().get<0>());
							top = std::min(top, a.min_corner().get<1>());
							bottom = std::max(bottom, a.max_corner().get<1>());

							for (auto jtr = std::begin(sparseBlocks); jtr != std::end(sparseBlocks); ++jtr)
								if (a.max_corner().get<1>() == jtr->max_corner().get<1>() && a.min_corner().get<0>() == jtr->min_corner().get<0>()
									&& a.min_corner().get<1>() == jtr->min_corner().get<1>() && a.max_corner().get<0>() == jtr->max_corner().get<0>())
								{
									sparseBlocks.erase(jtr);
									break;
								}
							isDeleted = true;
							rtree.remove(static_cast<box&>(a));

							rtree.insert(box(point(left, top), point(right, bottom)));
							this->sparseBlocks.emplace_back(point(left, top), point(right, bottom));
							left = top = INT_MAX;
							right = bottom = INT_MIN;
						}
						continue;
					}
					auto& a = result_n[i];
					left = std::min(left, a.min_corner().get<0>());
					right = std::max(right, a.max_corner().get<0>());
					top = std::min(top, a.min_corner().get<1>());
					bottom = std::max(bottom, a.max_corner().get<1>());

					for (auto jtr = std::begin(sparseBlocks); jtr != std::end(sparseBlocks); ++jtr)
						if (a.max_corner().get<1>() == jtr->max_corner().get<1>() && a.min_corner().get<0>() == jtr->min_corner().get<0>()
							&& a.min_corner().get<1>() == jtr->min_corner().get<1>() && a.max_corner().get<0>() == jtr->max_corner().get<0>())
						{
							sparseBlocks.erase(jtr);
							break;
						}
					rtree.remove(static_cast<box&>(a));
					isDeleted = true;
				}

				auto& b = result_n[result_n.size() - 1];
				left = std::min(left, b.min_corner().get<0>());
				right = std::max(right, b.max_corner().get<0>());
				top = std::min(top, b.min_corner().get<1>());
				bottom = std::max(bottom, b.max_corner().get<1>());

				for (auto jtr = std::begin(sparseBlocks); jtr != std::end(sparseBlocks); ++jtr)
					if (b.max_corner().get<1>() == jtr->max_corner().get<1>() && b.min_corner().get<0>() == jtr->min_corner().get<0>()
						&& b.min_corner().get<1>() == jtr->min_corner().get<1>() && b.max_corner().get<0>() == jtr->max_corner().get<0>())
					{
						sparseBlocks.erase(jtr);
						break;
					}
				rtree.remove(static_cast<box&>(b));
				isDeleted = true;
				if (left != INT_MAX)
				{
					rtree.insert(box(point(left, top), point(right, bottom)));
					this->sparseBlocks.emplace_back(point(left, top), point(right, bottom));
					left = top = INT_MAX;
					right = bottom = INT_MIN;
				}

				for (const auto& a : result_n)
				{
					for (auto jtr = std::begin(l); jtr != std::end(l); ++jtr)
						if (a.max_corner().get<1>() == jtr->max_corner().get<1>() && a.min_corner().get<0>() == jtr->min_corner().get<0>()
							&& a.min_corner().get<1>() == jtr->min_corner().get<1>() && a.max_corner().get<0>() == jtr->max_corner().get<0>())
						{
							l.erase(jtr);
							break;
						}

				}
				itr = std::begin(l);
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

		for (auto& q : rtree)
		{
			offsetX = q.min_corner().get<0>();
			offsetY = q.min_corner().get<1>();
			edgeX = q.max_corner().get<0>();
			edgeY = q.max_corner().get<1>();
			for (int i = offsetY; i < edgeY; ++i)
			{
				for (int j = offsetX; j < edgeX; j++)
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







	static double getKmeansBoundary(std::vector<int>& v)
	{
		std::vector<int> forCluster;
		for (const auto& p : v)
		{
			forCluster.emplace_back(p);
		}

		if (forCluster.empty())
		{
			// no meaning return value.
			return 0;
		}

		std::vector<TableDetection::KmeansType> clustered(forCluster.size());
		// for get 1/4th value, 3/4th value
		std::vector<int> forClusterTemp{ forCluster };

		std::sort(begin(forClusterTemp), end(forClusterTemp));

		double currentLow = static_cast<double>(forClusterTemp[0]);
		double currentUpper = static_cast<double>(forClusterTemp[forClusterTemp.size() - 1]);
		double lower, upper;

		// actually this while loop must divide as 2 group(xCluster, yCluser) but... just my tiresome
		do
		{
			int lowCnt = 0, upperCnt = 0;
			lower = currentLow;
			upper = currentUpper;
			currentUpper = currentLow = 0;
			for (int i = 0; i < forCluster.size(); i++)
			{
				clustered[i] = (abs(lower - forCluster[i]) >= abs(upper - forCluster[i])) ?
					TableDetection::KmeansType::TYPE_UPPER : TableDetection::KmeansType::TYPE_LOWER;
				// if clustered as lower
				if (clustered[i] == TableDetection::KmeansType::TYPE_LOWER)
				{
					currentLow += forCluster[i];
					++lowCnt;
				}
				else
				{
					currentUpper += forCluster[i];
					++upperCnt;
				}
			}
			if (lowCnt > 0) currentLow /= lowCnt;
			if (upperCnt > 0) currentUpper /= upperCnt;
		} while (lower != currentLow || upper != currentUpper);

		int lowerMaxValue = INT_MIN;
		int upperMinValue = INT_MAX;
		std::vector<int> lowerVector;
		for (int i = 0; i < clustered.size(); i++)
		{
			if (clustered[i] == TableDetection::KmeansType::TYPE_LOWER && forCluster[i] > lowerMaxValue)
				lowerMaxValue = forCluster[i], lowerVector.push_back(forCluster[i]);
			else if (clustered[i] == TableDetection::KmeansType::TYPE_UPPER && forCluster[i] < upperMinValue)
				upperMinValue = forCluster[i];
		}
		if (lowerMaxValue == INT_MIN || upperMinValue == INT_MAX)
			return INT_MAX;
		std::sort(std::begin(forCluster), std::end(forCluster));

		return static_cast<double>(lowerVector[lowerVector.size() / 2]);
	}
	std::vector<SparseBlock> SparseBlockManager::getSparseBlocksInRange(
		int top, int bottom, int left, int right)
	{
		std::vector<SparseBlock> result_n;
		rtree.query(bgi::intersects(box(point(left, top), point(right, bottom))), std::back_inserter(result_n));
		return result_n;
	}
}