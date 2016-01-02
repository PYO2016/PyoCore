#include "stdafx.h"
#include "CppUnitTest.h"
#include <memory>

#define private public
#include "../../PyoCore/Common/PngImage.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace PngImageTest
{		
	TEST_CLASS(PngImageTest)
	{
	public:
		
		TEST_METHOD(TestCase1)
		{
			// TODO: �׽�Ʈ �ڵ带 ���⿡ �Է��մϴ�.
			std::shared_ptr<Common::PngImage> img = Common::PngImage::LoadImage(L"C:/Users/taeguk/Pictures/test.png");
			img->storeToFile(L"C:/Users/taeguk/Pictures/test_copy.png");

			for (int i = 0; i < img->getHeight(); i++) {
				for (int j = 0; j < img->getWidth(); j++) {
					(*img)[i][j].R = 100;
					(*img)[i][j].G = 100;
					(*img)[i][j].B = 100;
				}
			}
			img->storeToFile(L"C:/Users/taeguk/Pictures/test_gray.png"); 
		}

	};
}