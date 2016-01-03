#pragma once

#include <string>
#include <memory>
#include "../Common/SparseBlock.h"
#include "../Common/SparseBlockManager.h"
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
		bool xycut(std::vector<Common::Line>& lines, unsigned areaWidth, unsigned areaHeight,
			unsigned offsetWidth, unsigned offsetHeight, bool edgeExist);	// not tested
		bool xycutPostProcess(std::vector<Common::Line>& lines,
			unsigned areaWidth, unsigned areaHeight, unsigned offsetWidth, unsigned offsetHeight);
		/* etc... */

	private:
		std::wstring imageFile, outputFile;
		bool isDebug;
		std::shared_ptr<Common::PngImage> pImage;
		std::shared_ptr<Common::SparseBlockManager> pSbm;
		std::shared_ptr<HistogramManager> pHm;
		Common::Table table;
		std::wstring result;

		/* Constants for recursive table detection. */
		/* These values is determined in preprocess(). */
		unsigned maxRecDepth;
		int minWidth;
		int minHeight;
	};
}