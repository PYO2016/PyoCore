#include "stdafx.h"
#include "CppUnitTest.h"
#include <iostream>

/* For test private methods and access to private variables */
#define private public

#include "../../PyoCore/TableDetection/HistogramManager.h"
#include "../../PyoCore/Common/PngImage.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace HistogramManagerTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		/* Tests for Histogram */

		TEST_METHOD(TestCalculateValues)
		{
			// TODO: �׽�Ʈ �ڵ带 ���⿡ �Է��մϴ�.
		}

		TEST_METHOD(TestApplyMedianFilter)
		{
			// TODO: �׽�Ʈ �ڵ带 ���⿡ �Է��մϴ�.
			TableDetection::Histogram histogram = getHistogramFromTrash();

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

		TableDetection::Histogram getHistogramFromTrash(void)
		{
			return *reinterpret_cast<TableDetection::Histogram*>(trashHistogram[cntHistogram++]);
		}
		TableDetection::HistogramManager getHistogramManagerFromTrash(void)
		{
			return *reinterpret_cast<TableDetection::HistogramManager*>(trashHistogramManager[cntHistogramManager++]);
		}

		char trashHistogram[100][sizeof(TableDetection::Histogram)];
		int cntHistogram = 0;
		char trashHistogramManager[100][sizeof(TableDetection::HistogramManager)];
		int cntHistogramManager = 0;
	};
}