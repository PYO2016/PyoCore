#include "stdafx.h"
#include "CppUnitTest.h"
#include <iostream>
#include <tuple>

/* For test private methods and access to private variables */
#define private public

#include "../../PyoCore/TableDetection/HistogramManager.h"
#include "../../PyoCore/Common/PngImage.h"

#include "../../PyoCore/TableDetection/TableExporter.h"
#include "../../PyoCore/Common/Table.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace HistogramManagerTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		/* Tests for Histogram */

		TEST_METHOD(TestCalculateValues)
		{
			// TODO: 테스트 코드를 여기에 입력합니다.
			auto img = Common::PngImage::LoadImage(L"C:/Users/KGWANGMIN/git/PYO2016/PyoCore/Tests/HistogramManagerTest/test.png");
			std::vector<Common::Cell> cells;
			Logger::WriteMessage("Test Started\n");
			if (img == nullptr)
			{
				Logger::WriteMessage("There's no input\n");
				return;
			}
			TableDetection::HistogramManager h(*img);
			h.cleanup();
			h.makeHistogram(TableDetection::HistogramType::TYPE_X);
			h.makeHistogram(TableDetection::HistogramType::TYPE_Y);
			//h.applyMedianFilter(TableDetection::HistogramType::TYPE_X);
			//h.applyMedianFilter(TableDetection::HistogramType::TYPE_Y);
			h.filterExtremum(TableDetection::HistogramType::TYPE_X);
			h.filterExtremum(TableDetection::HistogramType::TYPE_Y);
			h.applyKmeans(TableDetection::HistogramType::TYPE_X);
			h.applyKmeans(TableDetection::HistogramType::TYPE_Y);
			/*
			for (auto p : h.getTableInfo())
			{
				auto q1 = (std::get<0>(p));
				auto q2 = (std::get<1>(p));
				auto q3 = (std::get<2>(p));
				auto q4 = (std::get<3>(p));

				cells.emplace_back(q1, q2, q3, q4, L"");

				//Logger::WriteMessage((q1 + " " + q2 + " " + q3 + " " + q4 + "\n").c_str());
			}
			*/
			Assert::IsTrue(false);	// this unit test must be modified.
			std::wstring html = TableDetection::TableExporter::ExportTable(cells);
			Logger::WriteMessage(html.c_str());
		}

		TEST_METHOD(TestApplyMedianFilter)
		{
			// TODO: 테스트 코드를 여기에 입력합니다.

			/* Get trash object and initializing member variables as needed. */
			TableDetection::Histogram& histogram = getHistogramFromTrash();
			histogram.values = std::vector<int>();

			/* Test Case #1 */
			histogram.length = 10;
			histogram.values = { 1,10,3,40,3,20,1,20,3,40 };

			Logger::WriteMessage("Test Case #1\n");
			Logger::WriteMessage("-------- before --------\n");
			dumpVector(histogram.values);

			histogram.applyMedianFilter();

			Logger::WriteMessage("-------- after --------\n");
			dumpVector(histogram.values);

			/* Test Case #2 */
			histogram.length = 20;
			histogram.values = { 0,100,0,20,0,30,0,40,0,50,0,60,0,234,2,3,4,51,2,3 };

			Logger::WriteMessage("\nTest Case #2\n");
			Logger::WriteMessage("-------- before --------\n");
			dumpVector(histogram.values);

			histogram.applyMedianFilter();

			Logger::WriteMessage("-------- after --------\n");
			dumpVector(histogram.values);
		}

		/* Tests for HistogramManager */

	private:
		/* Useful Methods for test */

		/* Dump vector */
		template <typename T>
		void dumpVector(const std::vector<T> &vec)
		{
			bool first = true;
			for (const auto &elem : vec) {
				if (!first) Logger::WriteMessage(", ");
				Logger::WriteMessage(std::to_string(elem).c_str());
				first = false;
			}
			Logger::WriteMessage("\n");
		}

		/* Get Trash Object without calling constructor.
			Trash Object is not initialized. Must be initialized after acquirement.
			Be careful to use. There is a possibility to not working.
		*/
		/* Get Trash Histogram */
		TableDetection::Histogram& getHistogramFromTrash(void)
		{
			trashHistograms.emplace_back();
			return reinterpret_cast<TableDetection::Histogram&>(trashHistograms.back());
		}
		/* Get Trash HistogramManager */
		TableDetection::HistogramManager& getHistogramManagerFromTrash(void)
		{
			trashHistogramManagers.emplace_back();
			return reinterpret_cast<TableDetection::HistogramManager&>(trashHistogramManagers.back());
		}

		/* Trash version of Histogram and HistogramManager */
		struct TrashHistogram { char d[sizeof(TableDetection::Histogram)]; };
		struct TrashHistogramManager { char d[sizeof(TableDetection::HistogramManager)]; };

		std::vector<TrashHistogram> trashHistograms;
		std::vector<TrashHistogramManager> trashHistogramManagers;
	};
}