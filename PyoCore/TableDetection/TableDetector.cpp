#include "TableDetector.h"
#include "HistogramManager.h"
#include "../Preprocessing/Preprocessor.h"
#include <iostream>
#include "TableExporter.h"
#include <algorithm>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>

#define DEBUG_MSG(STR) if(isDebug) { \
	std::cerr << "[Debug] " STR << std::endl; \
}

#define DEBUG_ACTION(ACT) if(isDebug) { \
	ACT;	\
}

namespace TableDetection
{
	/*
	namespace bg = boost::geometry;
	namespace bgi = boost::geometry::index;

	typedef bg::model::point<int, 2, bg::cs::cartesian> point;
	typedef bg::model::box<point> box;
	*/
	/* TableDetector */

	TableDetector::TableDetector()
		:pImage(nullptr), pHm(nullptr), pSbm(nullptr)
	{
	}

	TableDetector::~TableDetector()
	{
		cleanup();
	}

	void TableDetector::cleanup()
	{
		pImage.reset();
		pSbm.reset();
		pHm.reset();
	}

	bool TableDetector::process(const std::wstring& imageFile, const std::wstring& outputFile, bool isDebug)
	{
		DEBUG_MSG("process() start!!");

		/* cleanup and parameter recognition */

		cleanup();

		this->imageFile = imageFile;
		this->outputFile = outputFile;
		this->isDebug = isDebug;

		/* real process!! */
		return process();
	}

	bool TableDetector::process()
	{
		DEBUG_MSG("(real) process() start!!");

		DEBUG_MSG("registerImage() start!!");

		/////// registerImage()
		if (!registerImage(imageFile))
			return false;

		DEBUG_MSG("registerImage() finish!!");

		DEBUG_MSG("preprocess() start!!");

		/////// preprocess()
		if (!preprocess())
			return false;

		DEBUG_MSG("preprocess() finish!!");

		DEBUG_MSG("detectTable() start!!");

		/////// detectTable()
		if (!detectTable())
			return false;

		DEBUG_MSG("detectTable() finish!!");

		DEBUG_MSG("exportTable() start!!");

		result = TableExporter::ExportTable(table);

		DEBUG_MSG("exportTable() finish!!");

		/* Do more things */

		DEBUG_MSG("(real) process() finish!!");

		return true;
	}

	bool TableDetector::registerImage(std::wstring imageFile)
	{
		cleanup();
		if ((pImage = Common::PngImage::LoadImage(imageFile)) == nullptr)
			return false;
		return true;
	}
	
	bool TableDetector::preprocess(void)
	{
		bool success = false;
		
		// do image pre-processing.
		if (!Preprocessing::Preprocessor::process(*pImage))
			goto END;

		DEBUG_MSG("After preprocessing, store image file");
		DEBUG_ACTION(pImage->storeToFile(imageFile + L"_after_preprocessing.png"));

		// get sparse blocks.
		pSbm = std::make_shared<Common::SparseBlockManager>(*pImage);
		
		if (!pSbm->makeSparseBlock())
			goto END;

		if (!pSbm->mergeSparseBlock())
			goto END;

		{
			auto p = pSbm->getSparseBlocks();
			for (auto pit : p)
			{
				auto n = pit.getBottom();
				auto m = pit.getRight();
				for (auto i = pit.getTop(); i < n; ++i)
				{
					for (auto j = pit.getLeft(); j < m; ++j)
					{
						(*pImage)[i][j].B = (*pImage)[i][j].G = (*pImage)[i][j].R = 0;
					}
				}
			}
		}

		DEBUG_MSG("After sparse block detection, store image file");
		DEBUG_ACTION(pImage->storeToFile(imageFile + L"_after_sparseblock.png"));

		// determine constants.
		// 1. minWidth = avg letter size * 1.5
		// 2. minHeight = avg letter size * 1.5
		// 3. maxRecDepth = ??
		minWidth = 10;
		minHeight = 10;
		maxRecDepth = 1;

		success = true;
	END:;
		return success;
	}

	/* detectTable is incomplete and inefficient. */
	/* These methods must be re-designed. */

	bool TableDetector::detectTable(void)
	{
		pHm = std::make_shared<HistogramManager>(*pImage);
		if (pImage->getWidth() < minWidth || pImage->getHeight() < minHeight) {
			// nothing to process.
			return true;
		}
		return recXycut(0, pImage->getWidth(), pImage->getHeight(), 0, 0);
	}

	bool TableDetector::recXycut(int recDepth, unsigned areaWidth, unsigned areaHeight,
		unsigned offsetWidth, unsigned offsetHeight)
	{
		if (recDepth >= maxRecDepth || areaWidth < minWidth || areaHeight < minHeight) {
			table.addCell(offsetHeight, offsetHeight + areaHeight - 1,
				offsetWidth, offsetWidth + areaWidth - 1);
			return true;
		}

		std::vector<std::tuple<int, int, int, int>> cells;

		if (!xycut(cells, areaWidth, areaHeight, offsetWidth, offsetHeight, recDepth > 0))
			return false;

		if (!xycutPostProcess(cells, offsetWidth, offsetHeight))
			return false;

		// when can't split cell anymore.
		if (cells.size() <= 1) {
			table.addCell(offsetHeight, offsetHeight + areaHeight - 1,
				offsetWidth, offsetWidth + areaWidth - 1);
			return true;
		}

		bool success = true;

		for (const auto &cell : cells) {
			int top = std::get<0>(cell) + offsetHeight;
			int bottom = std::get<1>(cell) + offsetHeight;
			int left = std::get<2>(cell) + offsetWidth;
			int right = std::get<3>(cell) + offsetWidth;

			success = success && recXycut(recDepth + 1, right - left + 1, bottom - top + 1, left, top);
			if (!success)
				break;
		}

		return success;
	}

	bool TableDetector::xycut(std::vector<std::tuple<int, int, int, int>>& cells, unsigned areaWidth, unsigned areaHeight,
		unsigned offsetWidth, unsigned offsetHeight, bool edgeExist)
	{
		bool success = false;

		pHm->setAttr(areaWidth, areaHeight, offsetWidth, offsetHeight, edgeExist);

		if (!pHm->makeHistogram(HistogramType::TYPE_X))
			goto END;
		if (!pHm->makeHistogram(HistogramType::TYPE_Y))
			goto END;

		if (!pHm->detectVisibleLines(HistogramType::TYPE_X))
			goto END;
		if (!pHm->detectVisibleLines(HistogramType::TYPE_Y))
			goto END;

		if (!pHm->applyMedianFilter(HistogramType::TYPE_X))
			goto END;
		if (!pHm->applyMedianFilter(HistogramType::TYPE_Y))
			goto END;

		if (!pHm->filterExtremum(HistogramType::TYPE_X))
			goto END;
		if (!pHm->filterExtremum(HistogramType::TYPE_Y))
			goto END;

		if (!pHm->applyKmeans(HistogramType::TYPE_X))
			goto END;
		if (!pHm->applyKmeans(HistogramType::TYPE_Y))
			goto END;

		cells = pHm->getTableInfo();

		success = true;
	END:;
		pHm->cleanup();

		return success;
	}

	bool TableDetector::xycutPostProcess(std::vector<std::tuple<int, int, int, int>>& cells, 
		unsigned offsetWidth, unsigned offsetHeight)
	{
		bool success = false;
		bgi::rtree<box, bgi::quadratic<16>> rtree;
		const std::list<Common::SparseBlock> &sparseBlockList = this->pSbm->getSparseBlocks();

		for (const auto &cell : cells) {
			int top = std::get<0>(cell) + offsetHeight;
			int bottom = std::get<1>(cell) + offsetHeight;
			int left = std::get<2>(cell) + offsetWidth;
			int right = std::get<3>(cell) + offsetWidth;
			rtree.insert(box(point(left, top), point(right, bottom)));
		}
		
		for (const auto &block : sparseBlockList) {
			std::vector<box> result;
			rtree.query(bgi::intersects(static_cast<const box&>(block)), std::back_inserter(result));
			int minTop = INT_MAX, maxBottom = INT_MIN, minLeft = INT_MAX, maxRight = INT_MIN;
			for (const auto &r : result) {
				int top = r.min_corner().get<1>();
				int bottom = r.max_corner().get<1>();
				int left = r.min_corner().get<0>();
				int right = r.max_corner().get<0>();
				minTop = std::min(top, minTop);
				maxBottom = std::max(bottom, maxBottom);
				minLeft = std::min(left, minLeft);
				maxRight = std::max(right, maxRight);
				rtree.remove(r);
			}
			if(minTop != INT_MAX)
				rtree.insert(box(point(minLeft, minTop), point(maxRight, maxBottom)));
		}

		cells.clear();
		for (const auto &r : rtree) {
			int top = r.min_corner().get<1>();
			int bottom = r.max_corner().get<1>();
			int left = r.min_corner().get<0>();
			int right = r.max_corner().get<0>();
			cells.emplace_back(top, bottom, left, right);
		}

		/*
		std::set<int, std::less<int>> horLineSet, verLineSet;
		int maxBottom = 0, maxRight = 0;
		for (const auto &cell : cells) {
			int top = std::get<0>(cell) + offsetHeight;
			int bottom = std::get<1>(cell) + offsetHeight;
			int left = std::get<2>(cell) + offsetWidth;
			int right = std::get<3>(cell) + offsetWidth;

			horLineSet.emplace(top);
			verLineSet.emplace(left);

			if (maxBottom < bottom)
				maxBottom = bottom;
			if (maxRight < right)
				maxRight = right;
		}
		horLineSet.emplace(maxBottom);
		verLineSet.emplace(maxRight);
		*/

		success = true;
	END:;
		return success;
	}
}