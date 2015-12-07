#include "TableDetector.h"
#include "HistogramManager.h"

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

	bool TableDetector::process(std::wstring imageFile, std::wstring outputFile)
	{
		if (!registerImage(imageFile))
			return false;

		if (!preprocess())
			return false;

		if (!makeHistogram())
			return false;

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