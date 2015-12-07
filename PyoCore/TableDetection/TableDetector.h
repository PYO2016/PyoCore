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

		bool process(std::wstring imageFile, std::wstring outputFile);

	private:
		bool registerImage(std::wstring imageFile);
		bool preprocess(void);
		bool makeHistogram(void);
		/* etc... */

	private:
		Common::PngImage* image;
		HistogramManager* hm;
	};
}