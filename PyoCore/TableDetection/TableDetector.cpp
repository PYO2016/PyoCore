#include "TableDetector.h"
#include "HistogramManager.h"
#include "../Preprocessing/Preprocessor.h"
#include <iostream>
#include "TableExporter.h"

#define DEBUG_MSG(STR) if(isDebug) { \
	std::cerr << "[Debug] " STR << std::endl; \
}

#define DEBUG_ACTION(ACT) if(isDebug) { \
	ACT;	\
}

namespace TableDetection
{

	/* TableDetector */

	TableDetector::TableDetector()
		:pImage(nullptr), pHm(nullptr)
	{
	}

	TableDetector::~TableDetector()
	{
		cleanup();
	}

	void TableDetector::cleanup()
	{
		pImage.reset();
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

		DEBUG_MSG("After preprocessing, store image file");
		DEBUG_ACTION(pImage->storeToFile(imageFile + L"_after_preprocessing.png"));

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
		return Preprocessing::Preprocessor::process(*pImage);
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

		/*
		bool valid = true;
		for (const auto &cell : cells) {
			int width = std::get<3>(cell) - std::get<2>(cell) + 1;
			int height = std::get<1>(cell) - std::get<0>(cell) + 1;
			if (width < minWidth || height < minHeight) {
				valid = false;
				break;
			}
		}
		*/

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

	/* etc... */
}