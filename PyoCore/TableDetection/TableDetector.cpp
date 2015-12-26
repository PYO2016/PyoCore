#include "TableDetector.h"
#include "HistogramManager.h"
#include "../Preprocessing/Preprocessor.h"
#include <iostream>

#define DEBUG_MSG(STR) if(isDebug) { \
	std::cout << "[Debug] " STR << std::endl; \
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

	/* detectTable and _detectTable is incomplete and inefficient. */
	/* These methods must be re-designed. */

	bool TableDetector::detectTable(void)
	{
		pHm = std::make_shared<HistogramManager>(*pImage);
		return _detectTable(0, pImage->getWidth(), pImage->getHeight(), 0, 0);
	}

	bool TableDetector::_detectTable(int recDepth, unsigned areaWidth, unsigned areaHeight,
		unsigned offsetWidth, unsigned offsetHeight)
	{
		if (recDepth >= maxRecDepth || areaWidth < minWidth || areaHeight < minHeight) {
			// register cell to this->table.
			// will be implemented...
			return true;
		}

		std::vector<std::tuple<int, int, int, int>> cells;

		if (!makeHistogram(cells, areaWidth, areaHeight, offsetWidth, offsetHeight))
			return false;

		// when not splited.
		if (cells.size() <= 1) {
			// register cell to this->table.
			// will be implemented...
			return true;
		}

		bool success = true;

		for (const auto &cell : cells) {
			int top = std::get<0>(cell);
			int bottom = std::get<1>(cell);
			int left = std::get<2>(cell);
			int right = std::get<3>(cell);

			success = success && _detectTable(recDepth + 1, right - left, bottom - top, left, top);
			if (!success)
				break;
		}

		return success;
	}

	bool TableDetector::makeHistogram(std::vector<std::tuple<int, int, int, int>>& cells, unsigned areaWidth, unsigned areaHeight,
		unsigned offsetWidth, unsigned offsetHeight)
	{
		bool success = false;

		pHm->setAttr(areaWidth, areaHeight, offsetWidth, offsetHeight);

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