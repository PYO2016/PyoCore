#pragma once

#include <string>
#include "../Common/PngImage.h"
#include "HistogramManager.h"

namespace TableDetection
{
	class TableDetector
	{
	public:
		TableDetector();
		~TableDetector();

		void cleanup();

		bool process(std::string imageFile);

	private:
		bool registerImage(std::string imageFile);
		bool preprocess(void);
		bool makeHistogram(void);
		/* etc... */

	private:
		Common::PngImage* image;
		HistogramManager* hm;
	};
}