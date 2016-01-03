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
	namespace bg = boost::geometry;
	namespace bgi = boost::geometry::index;

	typedef bg::model::point<int, 2, bg::cs::cartesian> point;
	typedef bg::model::box<point> box;

	class SparseBlockManager
	{
	public:
		SparseBlockManager(Common::PngImage& image);
		~SparseBlockManager();
		bool makeSparseBlock();
		bool mergeSparseBlock();
		inline double getSparseBlockHeightAvg();
		inline double getSparseBlockWidthAvg();
		inline double getLetterHeightAvg();
		inline double getLetterWidthAvg();

		inline std::list<SparseBlock>& getSparseBlocks();
		inline const std::list<SparseBlock>& getSparseBlocks() const;

	private:
		bool clearSparseBlocks();

		std::vector<SparseBlock> _sparseBlocks;
		std::list<SparseBlock> sparseBlocks;
		PngImage& image;
		boost::geometry::index::rtree<box, boost::geometry::index::quadratic<16>> rtree;
		box* covered = nullptr;

		double sparseBlockHeightAvg = -1, sparseBlockWidthAvg = -1;
		double letterHeightAvg = -1, letterWidthAvg = -1;

		const int directions[8][2] = { { 1, 0 },{ 0, 1 },{ -1, 0 },{ 0, -1 },{ 1, 1 },{ 1, -1 },{ -1, 1 },{ -1, -1 } };
		const int directionsLength = 8;
	};

	/* inline functions */

	inline double SparseBlockManager::getSparseBlockWidthAvg()
	{
		return this->sparseBlockWidthAvg;
	}
	inline double SparseBlockManager::getSparseBlockHeightAvg()
	{
		return this->sparseBlockHeightAvg;
	}
	inline double SparseBlockManager::getLetterWidthAvg()
	{
		return this->letterWidthAvg;
	}
	inline double SparseBlockManager::getLetterHeightAvg()
	{
		return this->letterHeightAvg;
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