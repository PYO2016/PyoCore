#include "TableDetector.h"
#include "HistogramManager.h"
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
		if (pImage) {
			pImage.reset();
		}
		if (pHm) {
			pHm.reset();
		}
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

		DEBUG_MSG("makeHistogram() start!!");

		/////// makeHistogram()
		if (!makeHistogram())
			return false;

		DEBUG_MSG("makeHistogram() finish!!");

		/* Do more things */

		DEBUG_MSG("(real) process() finish!!");

		return true;
	}

	bool TableDetector::registerImage(std::wstring imageFile)
	{
		cleanup();
		return (pImage = Common::PngImage::LoadImage(imageFile)) != nullptr;
	}

	bool TableDetector::preprocess(void)
	{
		/* do something. */

		/* not yet implemented */
		return false;
	}

	bool TableDetector::makeHistogram(void)
	{
		std::shared_ptr<HistogramManager> hm(new HistogramManager(*pImage));

		hm->makeHistogram(HistogramType::TYPE_X);
		hm->makeHistogram(HistogramType::TYPE_Y);

		hm->applyMedianFilter(HistogramType::TYPE_X);
		hm->applyMedianFilter(HistogramType::TYPE_Y);

		/* do more things. */

		// this->hm = hm;

		/* not yet implemented */
		return false;
	}

	/* etc... */
}