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

		bool process(const std::wstring& imageFile, std::wstring& resultString, bool isDebug);

	private:
		bool process(std::wstring& resultString);		// real process function!
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
		std::wstring imageFile;
		bool isDebug;
		std::shared_ptr<Common::PngImage> pImage;
		std::shared_ptr<Common::SparseBlockManager> pSbm;
		std::shared_ptr<HistogramManager> pHm;
		Common::Table table;

		/* For Debugging */
		std::shared_ptr<Common::PngImage> pResultImage;
		const int recColorCnt = 5;
		const unsigned recColor[5] =
		{
			/* http://ha927.com.ne.kr/color.html */
			/*
			0xd2b48c,	// burlywood
			0xf4a460,	// sandybrown
			0xdaa520,	// goldenrod
			0xb8860b,	// darkgoldenrod
			*/
			0xdc143c,	// crimson
			//0xff4500,	// orangered
			//0xffd700,	// gold
			//0xdaa520,	// goldenrod
			0xff8c00,	// darkorange
			0x4169e1,	// royalblue
			0x483d7b,	// darkslateblue
			0x00008b	// darkblue
		};

		/* Constants for recursive table detection. */
		/* These values is determined in preprocess(). */
		unsigned maxRecDepth;
		int minRecWidth;
		int minRecHeight;
	};
}