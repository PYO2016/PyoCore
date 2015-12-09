#pragma once

#include <string>
#include <memory>
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

		bool process(const std::wstring& imageFile, const std::wstring& outputFile, bool isDebug);

	private:
		bool process();		// real process function!
		bool registerImage(std::wstring imageFile);
		bool preprocess(void);
		bool makeHistogram(void);
		/* etc... */

	private:
		std::wstring imageFile, outputFile;
		bool isDebug;
		std::shared_ptr<Common::PngImage> pImage;
		std::shared_ptr<HistogramManager> pHm;
	};
}