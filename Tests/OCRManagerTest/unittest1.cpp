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
			std::shared_ptr<PngImage> img = PngImage::_LoadImage(L"C:/Users/JK/Desktop/test/test006.png");

			list<SparseBlock> blocks;
			blocks.push_back(SparseBlock(point(5, 5), point(img->getWidth() - 5, img->getHeight() - 5)));

			OCRManager::recognize(*img, blocks);
			
			wstring text = blocks.front().getText();
			///
			Logger::WriteMessage("fuck lib");
			Logger::WriteMessage(text.c_str());
		}

	};
}