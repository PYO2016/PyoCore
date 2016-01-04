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
		bool process();

		inline double getSparseBlockHeightAvg();
		inline double getSparseBlockWidthAvg();
		inline double getLetterHeightAvg();
		inline double getLetterWidthAvg();
		bool hasCollisionWithSparseBlock(int top, int bottom, int left, int right);
		std::vector<SparseBlock> SparseBlockManager::getSparseBlocksInRange(
			int top, int bottom, int left, int right);

		inline std::list<SparseBlock>& getSparseBlocks();
		inline const std::list<SparseBlock>& getSparseBlocks() const;

		double letterWidthMin = -1;

	private:
		bool makeSparseBlock();
		bool mergeSparseBlock();
		bool initImageToZero();
		bool initImageWithSparseBlocks();
		bool clearSparseBlocks();
		bool arrangeSparseBlocks();

		std::list<SparseBlock> sparseBlocks;
		PngImage& image;
		boost::geometry::index::rtree<box, boost::geometry::index::quadratic<16>> rtree;

		double sparseBlockHeightAvg = -1, sparseBlockWidthAvg = -1;
		double letterHeightAvg = -1, letterWidthAvg = -1;

		const int directions[8][2] = { { 1, 0 },{ 0, 1 },{ -1, 0 },{ 0, -1 },{ 1, 1 },{ 1, -1 },{ -1, 1 },{ -1, -1 } };
		const int directionsLength = 8;
		//const int directions[24][2] = { { 2, 2 },{ 2, 1 },{ 2, 0 },{ 2, -1 },{ 2, -2 },{ 1, 2 },{ 1, 1 },{ 1, 0 },{ 1, -1 },{ 1, -2 },{ 0, 2 },{ 0, 1 },{ 0, -1 },{ 0, -2 },{ -1, 2 },{ -1, 1 },{ -1, 0 },{ -1, -1 },{ -1, -2 },{ -2, 2 },{ -2, 1 },{ -2, 0 },{ -2, -1 },{ -2, -2 } };
		//const int directionsLength = 24;
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