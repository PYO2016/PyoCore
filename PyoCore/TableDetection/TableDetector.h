#pragma once

#include "../Common/PngImage.h"

namespace TableDetection
{
	class TableDetector
	{
	public:
		TableDetector();

		void cleanup();

		bool process(const char *imageFile);

	private:
		bool registerImage(const char *imageFile);
		bool preprocess(void);
		bool makeHistogram(void);
		/* etc... */

	private:
		Common::PngImage* image;
	};
}