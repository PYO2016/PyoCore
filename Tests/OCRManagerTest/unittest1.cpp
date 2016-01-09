#include "stdafx.h"
#include "CppUnitTest.h"
#include "../../PyoCore/TableDetection/OCRManager.h"
#include "../../PyoCore/Common/SparseBlock.h"
#include "../../PyoCore/Common/PngImage.h"
#include <boost/geometry/geometries/point.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <tesseract\baseapi.h>
#include <leptonica\allheaders.h>
#include <list>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace cv;
using namespace TableDetection;
using namespace tesseract;
using namespace Common;

namespace OCRManagerTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestRecognize)
		{

		}

	};
}