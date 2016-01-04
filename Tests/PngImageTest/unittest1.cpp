#include "stdafx.h"
#include "CppUnitTest.h"
#include <memory>

#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"

#define private public
#include "../../PyoCore/Common/PngImage.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Common;

namespace PngImageTest
{		
	TEST_CLASS(PngImageTest)
	{
	public:
		
		TEST_METHOD(TestCase1)
		{
			// TODO: 테스트 코드를 여기에 입력합니다.
			std::shared_ptr<Common::PngImage> img = Common::PngImage::_LoadImage(L"C:/Users/JK/Desktop/sample/aa.png");

			PngImage copy(*img);

			cv::Mat* src = &(img->imageMat);

			cvtColor(*src, *src, CV_BGR2GRAY);

			img->storeToFile(L"C:/Users/JK/Desktop/sample/aa_gray.png");
			copy.storeToFile(L"C:/Users/JK/Desktop/sample/aa_copy.png");
		}

	};
}