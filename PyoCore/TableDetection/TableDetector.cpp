#include "TableDetector.h"
#include "HistogramManager.h"
#include "../Preprocessing/Preprocessor.h"
#include <iostream>
#include "TableExporter.h"
#include "OCRManager.h"
#include <algorithm>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/segment.hpp>
#include <boost/geometry/index/rtree.hpp>

#define DEBUG_MSG(STR) if(this->isDebug) { \
	std::cerr << "[Debug] " STR << std::endl; \
}

#define DEBUG_ACTION(ACT) if(this->isDebug) { \
	ACT;	\
}

/* We can do it. Start of revolution. */

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
		try
		{
			if (!registerImage(imageFile))
				return false;
		}
		catch (std::exception e) {
			resultString = L"registerImage ";
			resultString.append(Common::EncodingConverter::s2ws(e.what()));
			return false;
		}

		DEBUG_MSG("registerImage() finish!!");

		DEBUG_MSG("preprocess() start!!");
		this->pImage->setPadding(5);
		Common::PngImage imgOri(*(this->pImage));

		/////// preprocess()
		try
		{
			if (!preprocess())
				return false;
		}
		catch (std::exception e) {
			resultString = L"preprocess ";
			resultString.append(Common::EncodingConverter::s2ws(e.what()));
			return false;
		}

		DEBUG_MSG("preprocess() finish!!");

		DEBUG_MSG("detectTable() start!!");

		/////// detectTable()
		try
		{
			if (!detectTable())
				return false;
		}
		catch (std::exception e) {
			resultString = L"detectTable ";
			resultString.append(Common::EncodingConverter::s2ws(e.what()));
			return false;
		}
		DEBUG_MSG("detectTable() finish!!");

		DEBUG_MSG("ocr() start!!");
		try
		{
			int error = OCRManager::recognize(imgOri, table.getCells());
			switch (error) {
			case -1:
				resultString = L"recognize: can not allocate memory";
				return false;
			case -2:
				resultString = L"recognize: can not init tesseract";
				return false;
			case -3:
				resultString = L"recognize: ang dae";
				return false;
			default:
				break;
			}
		}
		catch (std::exception e) {
			resultString = L"recognize ";
			resultString.append(Common::EncodingConverter::s2ws(e.what()));
			return false;
		}
		DEBUG_MSG("ocr() finish!!");

		DEBUG_MSG("exportTable() start!!");
		try
		{
			resultString = TableExporter::ExportTable(table);
		}
		catch (std::exception e) {
			resultString = L"ExportTable ";
			resultString.append(Common::EncodingConverter::s2ws(e.what()));
			return false;
		}
		DEBUG_MSG("exportTable() finish!!");

		/* Do more things */

		DEBUG_MSG("(real) process() finish!!");

		return true;
	}

	bool TableDetector::registerImage(std::wstring imageFile)
	{
		cleanup();
		if ((pImage = Common::PngImage::LoadPngImage(imageFile)) == nullptr)
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
		minRecWidth = pSbm->getLetterWidthAvg() * 1.5;
		minRecHeight = pSbm->getLetterHeightAvg() * 1.5;
		maxRecDepth = 5;

		success = true;
	END:;
		return success;
	}

	/* detectTable is incomplete and inefficient. */
	/* These methods must be re-designed. */

	bool TableDetector::detectTable(void)
	{
		pHm = std::make_shared<HistogramManager>(*pImage);
		if (pImage->getWidth() < minRecWidth || pImage->getHeight() < minRecHeight) {
			// nothing to process.
			return true;
		}

		DEBUG_ACTION(pResultImage = std::make_shared<Common::PngImage>(*pImage));

		bool success = false;

		if (!recXycut(0, pImage->getWidth(), pImage->getHeight(), 0, 0))
			goto END;

		DEBUG_MSG("After table detection, store result file to image.");
		DEBUG_ACTION(pResultImage->storeToFile(imageFile + L"_result.png"));

		success = true;
	END:;

		return success;
	}

	bool TableDetector::recXycut(int recDepth, unsigned areaWidth, unsigned areaHeight,
		unsigned offsetWidth, unsigned offsetHeight)
	{
		if (recDepth >= maxRecDepth || areaWidth < minRecWidth || areaHeight < minRecHeight) {
			table.addCell(offsetHeight, offsetHeight + areaHeight - 1,
				offsetWidth, offsetWidth + areaWidth - 1);
			return true;
		}

		/// when no sparse block in cell. ///////////
		if (!this->pSbm->hasCollisionWithSparseBlock(offsetHeight, offsetHeight + areaHeight - 1,
			offsetWidth, offsetWidth + areaWidth - 1)) {
			table.addCell(offsetHeight, offsetHeight + areaHeight - 1,
				offsetWidth, offsetWidth + areaWidth - 1);
			return true;
		}

		bool firstRecursive = (recDepth == 0);
		std::vector<Common::Line> lines;

		/// notice : 5th arg is not used now...
		if (!xycut(lines, areaWidth, areaHeight, offsetWidth, offsetHeight, !firstRecursive))
			return false;

		/*
		if (!xycutPostProcess(lines, areaWidth, areaHeight, offsetWidth, offsetHeight))
			return false;
		*/

		const int adjHorLineConstant = this->pSbm->getLetterHeightAvg() * 1.2;
		const int adjVerLineConstant = this->pSbm->getLetterWidthAvg() * 1.2;
		std::list<Common::Line> horList, verList;
		std::pair<std::list<Common::Line>*, Common::LineType> lineListPairArray[2]
			= { { &horList, Common::LineType::LINE_HORIZONTAL },
			{ &verList, Common::LineType::LINE_VERTICAL } };

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

		// for security. "lines" must not be used after this line.
		lines.clear();

		if (!firstRecursive) {
			// select boundary 4 lines. And remove lines near boundary lines.
			for (auto lineListPair : lineListPairArray) {
				auto &lineList = *(lineListPair.first);
				auto lineType = lineListPair.second;
				const int adjLineConstant =
					(lineType == Common::LineType::LINE_HORIZONTAL ? adjHorLineConstant : adjVerLineConstant);
				unsigned boundary[2];

				switch (lineType) {
				case Common::LineType::LINE_HORIZONTAL:
					boundary[0] = offsetHeight;
					boundary[1] = offsetHeight + areaHeight - 1;
					break;
				case Common::LineType::LINE_VERTICAL:
					boundary[0] = offsetWidth;
					boundary[1] = offsetWidth + areaWidth - 1;
					break;
				default:
					return false;
				}

				unsigned base = boundary[0];
				while (!lineList.empty() &&
					lineList.front().getOffset() - base <= adjLineConstant) {
					base = lineList.front().getOffset();
					lineList.pop_front();
				}
				base = boundary[1];
				while (!lineList.empty() &&
					base - lineList.back().getOffset() <= adjLineConstant) {
					base = lineList.back().getOffset();
					lineList.pop_back();
				}
				lineList.emplace_front(lineType, boundary[0]);
				lineList.emplace_back(lineType, boundary[1]);
			}
		}

		// merge adjecent lines. (select middle thing)
		{
			for (auto lineListPair : lineListPairArray) {
				auto &lineList = *(lineListPair.first);
				auto lineType = lineListPair.second;
				const int adjLineConstant =
					(lineType == Common::LineType::LINE_HORIZONTAL ? adjHorLineConstant : adjVerLineConstant);

				auto itr = std::begin(lineList);
				while (itr != std::end(lineList)) {
					auto jtr = itr;
					auto ktr = std::next(jtr);
					while (ktr != std::end(lineList) && ktr->getOffset() - jtr->getOffset() <= adjLineConstant) {
						jtr = ktr;
						ktr = std::next(ktr);
					}
					int r = std::distance(itr, ktr) / 2, l = std::distance(itr, ktr) - r - 1;
					jtr = itr;
					while (l-- > 0)
						jtr = lineList.erase(jtr);
					jtr = std::next(jtr);
					while (r-- > 0)
						jtr = lineList.erase(jtr);
					itr = jtr;
				}
			}
		}

		/// when can't split cell anymore. ///////////
		if (std::size(horList) < 2 || std::size(verList) < 2) {
			table.addCell(offsetHeight, offsetHeight + areaHeight - 1,
				offsetWidth, offsetWidth + areaWidth - 1);
			return true;
		}


		int leftBoundary = verList.front().getOffset();
		int rightBoundary = verList.back().getOffset();
		int topBoundary = horList.front().getOffset();
		int bottomBoundary = horList.back().getOffset();

		auto curSparseBlocks = this->pSbm->getSparseBlocksInRange(
			topBoundary, bottomBoundary, leftBoundary, rightBoundary);
		bgi::rtree<box, bgi::quadratic<16>> sparseBlockRtree;

		for (const auto &curSparseBlock : curSparseBlocks) {
			sparseBlockRtree.insert(static_cast<box>(curSparseBlock));
		}

		// delete non-meaning lines.
		{
			for (auto lineListPair : lineListPairArray) {
				auto &lineList = *(lineListPair.first);
				auto lineType = lineListPair.second;

				auto itr = std::next(std::begin(lineList));
				while (itr != std::prev(std::end(lineList))) {
					int offset = itr->getOffset();
					point p1, p2;

					switch (lineType) {
					case Common::LineType::LINE_HORIZONTAL:
						p1 = point(rightBoundary, offset);
						p2 = point(leftBoundary, offset);
						break;
					case Common::LineType::LINE_VERTICAL:
						p1 = point(offset, bottomBoundary);
						p2 = point(offset, topBoundary);
						break;
					default:
						return false;
					}

					std::vector<box> result_n;
					sparseBlockRtree.query(bgi::intersects(box(point(leftBoundary, topBoundary), p1)),
						std::back_inserter(result_n));
					if (result_n.empty()) {
						itr = lineList.erase(itr);
						continue;
					}
					result_n.clear();
					sparseBlockRtree.query(bgi::intersects(box(p2, point(rightBoundary, bottomBoundary))),
						std::back_inserter(result_n));
					if (result_n.empty()) {
						itr = lineList.erase(itr);
						continue;
					}
					++itr;
				}
			}
		}

		// for debug
		if (recDepth >= 1 && std::size(horList) >= 2 && std::size(verList) >= 2 &&
			std::size(horList) + std::size(verList) >= 5) {
			int a = 3;
		}

		std::vector<Common::Cell> cells;

		/// when can't split cell anymore. ///////////
		if (std::size(horList) < 2 || std::size(verList) < 2) {
			table.addCell(offsetHeight, offsetHeight + areaHeight - 1,
				offsetWidth, offsetWidth + areaWidth - 1);
			return true;
		}

		// get cells by lines.
		int top = topBoundary, bottom, left, right;
		int hIdx = 0, vIdx;
		for (auto horLine = std::next(std::begin(horList)); horLine != std::end(horList); ++horLine) {
			bottom = horLine->getOffset();
			left = leftBoundary;
			vIdx = 0;
			for (auto verLine = std::next(std::begin(verList)); verLine != std::end(verList); ++verLine) {
				right = verLine->getOffset();
				cells.emplace_back(top, bottom, left, right);
				left = right;
				++vIdx;
			}
			top = bottom;
			++hIdx;
		}

		// merge cells.
		{
			bgi::rtree<box, bgi::quadratic<16>> cellRtree;

			// construct rtree.
			for (const auto &cell : cells) {
				cellRtree.insert(box(point(cell.getLeft(), cell.getTop()),
					point(cell.getRight(), cell.getBottom())));
			}

			// merge cells by sparse blocks.
			for (const auto &curSparseBlock : curSparseBlocks) {
				std::vector<box> result_n;
				cellRtree.query(bgi::intersects(static_cast<const box&>(curSparseBlock)),
					std::back_inserter(result_n));
				if (std::size(result_n) < 2)
					continue;
				int lb = INT_MAX, rb = INT_MIN, tb = INT_MAX, bb = INT_MIN;
				for (const auto &b : result_n) {
					if (lb > b.min_corner().get<0>())
						lb = b.min_corner().get<0>();
					if (rb < b.max_corner().get<0>())
						rb = b.max_corner().get<0>();
					if (tb > b.min_corner().get<1>())
						tb = b.min_corner().get<1>();
					if (bb < b.max_corner().get<1>())
						bb = b.max_corner().get<1>();
				}
				result_n.clear();

				cellRtree.query(bgi::intersects(box(point(lb + 1, tb + 1), point(rb - 1, bb - 1))),
					std::back_inserter(result_n));

				for (const auto &b : result_n) {
					cellRtree.remove(b);
				}
				cellRtree.insert(box(point(lb, tb), point(rb, bb)));
			}

			// RE : get cells by modified rtree.
			cells.clear();
			for (const auto &b : cellRtree) {
				int top = b.min_corner().get<1>();
				int bottom = b.max_corner().get<1>();
				int left = b.min_corner().get<0>();
				int right = b.max_corner().get<0>();

				cells.emplace_back(top, bottom, left, right);
			}
		}

		/// //////////////////////////////

		bool success = true;

		for (const auto &cell : cells) {
			success = success && recXycut(recDepth + 1,
				cell.getWidth(), cell.getHeight(), cell.getLeft(), cell.getTop());
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

			default:
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
			else {
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