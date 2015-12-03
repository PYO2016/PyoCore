#include "TableDetector.h"
#include "HistogramManager.h"

namespace TableDetection
{

	/* TableDetector */

	TableDetector::TableDetector()
		:image(NULL)
	{
	}

	void TableDetector::cleanup()
	{
		if (!image) {
			delete image;
			image = NULL;
		}
	}

	bool TableDetector::process(const char *imageFile)
	{
		if (!registerImage(imageFile))
			return false;

		if (!preprocess())
			return false;

		if (!makeHistogram())
			return false;

		return true;
	}

	bool TableDetector::registerImage(const char *imageFile)
	{
		cleanup();
		return (image = Common::PngImage::LoadImage(imageFile)) != NULL;
	}

	bool TableDetector::preprocess(void)
	{
		/* do something. */
		return false;
	}

	bool TableDetector::makeHistogram(void)
	{
		HistogramManager hm(*image);

		hm.makeHistogram(HistogramType::TYPE_X);
		hm.makeHistogram(HistogramType::TYPE_Y);

		/* do more things. */

		return false;
	}

	/* etc... */
}