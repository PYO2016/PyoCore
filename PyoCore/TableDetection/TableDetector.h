#pragma once

#include <string>
#include <memory>
#include "../Common/PngImage.h"
#include "../Common/Table.h"
#include "HistogramManager.h"
#include <tuple>

namespace TableDetection
{
	class TableDetector
	{
	public:
		TableDetector();
		TableDetector(const TableDetector&) = delete;
		TableDetector& operator=(const TableDetector&) = delete;
		~TableDetector();

		void cleanup();

		bool process(const std::wstring& imageFile, const std::wstring& outputFile, bool isDebug);

	private:
		bool process();		// real process function!
		bool registerImage(std::wstring imageFile);
		bool preprocess(void);
		bool detectTable(void);		// not tested
		bool recXycut(int recDepth, unsigned areaWidth, unsigned areaHeight,
			unsigned offsetWidth, unsigned offsetHeight);	// not tested
		bool xycut(std::vector<std::tuple<int, int, int, int>>& cells, unsigned areaWidth, unsigned areaHeight,
			unsigned offsetWidth, unsigned offsetHeight);	// not tested
		/* etc... */

	private:
		std::wstring imageFile, outputFile;
		bool isDebug;
		std::shared_ptr<Common::PngImage> pImage;
		std::shared_ptr<HistogramManager> pHm;
		Common::Table table;

		/* Constants for recursive table detection. */
		/* These values changed depending on image size. */
		unsigned maxRecDepth = 1;
		int minWidth = 10;
		int minHeight = 10;
	};
}