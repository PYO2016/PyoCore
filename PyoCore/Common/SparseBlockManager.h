#pragma once

#include "PngImage.h"
#include "SparseBlock.h"
#include <vector>
#include <list>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>

namespace Common
{
	typedef boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian> point;
	typedef boost::geometry::model::box<point> box;

	class SparseBlockManager
	{
	public:
		SparseBlockManager(Common::PngImage& image);
		~SparseBlockManager();
		bool makeSparseBlock();
		bool mergeSparseBlock();
		inline double getHeightAvg();
		inline double getWidthAvg();

		inline std::list<SparseBlock>& getSparseBlocks();
		inline const std::list<SparseBlock>& getSparseBlocks() const;

	private:
		bool clearSparseBlocks();
		bool initWidthHeight();

		std::vector<SparseBlock> _sparseBlocks;
		std::list<SparseBlock> sparseBlocks;
		PngImage& image;
		boost::geometry::index::rtree<box, boost::geometry::index::quadratic<16>> rtree;
		box* covered = nullptr;

		double heightAvg = -1, widthAvg = -1;

		const int directions[8][2] = { { 1, 0 },{ 0, 1 },{ -1, 0 },{ 0, -1 },{ 1, 1 },{ 1, -1 },{ -1, 1 },{ -1, -1 } };
		const int directionsLength = 8;
	};

	/* inline functions */

	inline double SparseBlockManager::getWidthAvg()
	{
		return this->widthAvg;
	}
	inline double SparseBlockManager::getHeightAvg()
	{
		return this->heightAvg;
	}
	inline std::list<SparseBlock>& SparseBlockManager::getSparseBlocks()
	{
		return this->sparseBlocks;
	}
	inline const std::list<SparseBlock>& SparseBlockManager::getSparseBlocks() const
	{
		return this->sparseBlocks;
	}
}