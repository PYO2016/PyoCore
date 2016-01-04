#include "TableDetector.h"
#include "HistogramManager.h"
#include "../Preprocessing/Preprocessor.h"
#include <iostream>
#include "TableExporter.h"
#include <algorithm>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/segment.hpp>
#include <boost/geometry/index/rtree.hpp>

#define DEBUG_MSG(STR) if(isDebug) { \
	std::cerr << "[Debug] " STR << std::endl; \
}

#define DEBUG_ACTION(ACT) if(isDebug) { \
	ACT;	\
}

namespace TableDetection
{
	namespace bg = boost::geometry;
	namespace bgi = boost::geometry::index;

	typedef bg::model::point<int, 2, bg::cs::cartesian> point;
	typedef bg::model::box<point> box;
	typedef bg::model::segment<point> segment;
	
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

	bool TableDetector::process(const std::wstring& imageFile, std::wstring& resultString, bool isDebug)
	{
		DEBUG_MSG("process() start!!");

		/* cleanup and parameter recognition */

		cleanup();

		this->imageFile = imageFile;
		this->isDebug = isDebug;

		/* real process!! */
		return process(resultString);
	}

	bool TableDetector::process(std::wstring& resultString)
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

		resultString = TableExporter::ExportTable(table);

		DEBUG_MSG("exportTable() finish!!");

		/* Do more things */

		DEBUG_MSG("(real) process() finish!!");

		return true;
	}

	bool TableDetector::registerImage(std::wstring imageFile)
	{
		cleanup();
		if ((pImage = Common::PngImage::_LoadImage(imageFile)) == nullptr)
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

		if (!pSbm->process())
			goto END;

		DEBUG_MSG("After sparse block detection, store image file");
		DEBUG_ACTION(pImage->storeToFile(imageFile + L"_after_sparseblock.png"));

		// determine constants.
		minWidth = pSbm->getLetterWidthAvg() * 1.5;
		minHeight = pSbm->getLetterHeightAvg() * 1.5;
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

		std::vector<Common::Line> lines;

		if (!xycut(lines, areaWidth, areaHeight, offsetWidth, offsetHeight, recDepth > 0))
			return false;

		if (!xycutPostProcess(lines, areaWidth, areaHeight, offsetWidth, offsetHeight))
			return false;

		// when can't split cell anymore.
		if (lines.empty()) {
			table.addCell(offsetHeight, offsetHeight + areaHeight - 1,
				offsetWidth, offsetWidth + areaWidth - 1);
			return true;
		}

		std::vector<Common::Cell> cells;
		std::list<Common::Line> horList, verList;

		std::sort(lines.begin(), lines.end(), [](const Common::Line &line1, const Common::Line &line2) {
			return line1.getOffset() < line2.getOffset();
		});

		for (const auto &line : lines) {
			switch (line.getType()) {
			case Common::LineType::LINE_HORIZONTAL:
				horList.push_back(std::move(line));
				break;

			case Common::LineType::LINE_VERTICAL:
				verList.push_back(std::move(line));
				break;

			default:
				return false;
			}
		}

		// for security.
		lines.clear();
		
		auto itr = std::begin(horList);
		while (itr != std::end(horList))
		{
			auto jtr = std::next(itr);
			if (jtr != std::end(horList))
			{
				if (jtr->getOffset() - itr->getOffset() <= 8)
				{
					horList.erase(jtr);
				}
				else
				{
					++itr;
				}
			}
			else
			{
				break;
			}
		}

		itr = std::begin(verList);
		while (itr != std::end(verList))
		{
			auto jtr = std::next(itr);
			if (jtr != std::end(verList))
			{
				if (jtr->getOffset() - itr->getOffset() <= 8)
				{
					verList.erase(jtr);
				}
				else
				{
					++itr;
				}
			}
			else
			{
				break;
			}
		}
		// some problems...
		//horList.emplace_back(Common::LineType::LINE_HORIZONTAL, offsetHeight + areaHeight - 1);
		//verList.emplace_back(Common::LineType::LINE_VERTICAL, offsetWidth + areaWidth - 1);

		if (!horList.empty() && !verList.empty()) {

			int top = horList.begin()->getOffset(), bottom, left, right;
			int initLeft = verList.begin()->getOffset();

			horList.erase(horList.begin());
			verList.erase(verList.begin());

			for (const auto &horLine : horList) {
				bottom = horLine.getOffset();
				left = initLeft;
				for (const auto &verLine : verList) {
					right = verLine.getOffset();
					cells.emplace_back(top, bottom, left, right);
					left = right;
				}
				top = bottom;
			}
		}

		bool success = true;

		for (const auto &cell : cells) {
			int top = cell.getTop();
			int bottom = cell.getBottom();
			int left = cell.getLeft();
			int right = cell.getRight();

			success = success && recXycut(recDepth + 1, right - left + 1, bottom - top + 1, left, top);
			if (!success)
				break;
		}

		return success;
	}

	bool TableDetector::xycut(std::vector<Common::Line>& lines, unsigned areaWidth, unsigned areaHeight,
		unsigned offsetWidth, unsigned offsetHeight, bool edgeExist)
	{
		bool success = false;

		pHm->setAttr(areaWidth, areaHeight, offsetWidth, offsetHeight, edgeExist);

		if (!pHm->makeHistogram(HistogramType::TYPE_X))
			goto END;
		if (!pHm->makeHistogram(HistogramType::TYPE_Y))
			goto END;

		if (!pHm->detectSpecialValues(HistogramType::TYPE_X))
			goto END;
		if (!pHm->detectSpecialValues(HistogramType::TYPE_Y))
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

		lines = pHm->getLineInfo();

		success = true;
	END:;
		pHm->cleanup();

		return success;
	}

	bool TableDetector::xycutPostProcess(std::vector<Common::Line>& lines,
		unsigned areaWidth, unsigned areaHeight, unsigned offsetWidth, unsigned offsetHeight)
	{
		bool success = false;
		bgi::rtree<segment, bgi::quadratic<16>> rtree;
		const std::list<Common::SparseBlock> &sparseBlockList = this->pSbm->getSparseBlocks();

		for (const auto &line : lines) {
			Common::LineType type = line.getType();
			int offset = line.getOffset();
			int top, bottom, left, right;
			
			switch (type) {
			case Common::LineType::LINE_HORIZONTAL:
				top = bottom = offset;
				left = offsetWidth;
				right = offsetWidth + areaWidth - 1;
				break;

			case Common::LineType::LINE_VERTICAL:
				left = right = offset;
				top = offsetHeight;
				bottom = offsetHeight + areaHeight - 1;
				break;

			default : 
				return false;
			}

			rtree.insert(segment(point(left, top), point(right, bottom)));
		}

		for (const auto &block : sparseBlockList) {
			std::vector<segment> results;
			rtree.query(bgi::intersects(static_cast<const box&>(block)), std::back_inserter(results));
			for (const auto &result : results) {
				rtree.remove(result);
			}
		}

		lines.clear();
		for (const auto &r : rtree) {
			int top = r.first.get<1>();
			int bottom = r.second.get<1>();
			int left = r.first.get<0>();
			int right = r.second.get<0>();

			Common::LineType type;
			int offset;
			if (top == bottom) {
				type = Common::LineType::LINE_HORIZONTAL;
				offset = top;
			}
			else{
				type = Common::LineType::LINE_VERTICAL;
				offset = left;
			}
			lines.emplace_back(type, offset);
		}

		success = true;
	END:;
		return success;
	}
}