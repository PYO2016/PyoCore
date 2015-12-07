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
		:image(NULL), hm(NULL)
	{
	}

	TableDetector::~TableDetector()
	{
		cleanup();
	}

	void TableDetector::cleanup()
	{
		if (!image) {
			delete image;
			image = NULL;
		}
		if (!hm) {
			delete hm;
			hm = NULL;
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
		DEBUG_ACTION(image->storeToFile(imageFile + L"_after_preprocessing.png"));

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
		return (image = Common::PngImage::LoadImage(imageFile)) != NULL;
	}

	bool TableDetector::preprocess(void)
	{
		/* do something. */

		/* not yet implemented */
		return false;
	}

	bool TableDetector::makeHistogram(void)
	{
		HistogramManager *hm = new HistogramManager(*image);

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