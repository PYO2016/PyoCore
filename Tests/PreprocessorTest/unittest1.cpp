#include "stdafx.h"
#include "CppUnitTest.h"
#include "../../PyoCore/Common/PngImage.h"
#include "../../../Leptonica/include/leptonica/allheaders.h"
#include "../../PyoCore/Preprocessing/Preprocessor.h"
#include "../../PyoCore/Lodepng/lodepng.h"
#include <vector>
#include <string>

#pragma comment(lib, "../../../Leptonica/lib/liblept168")

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Common;
using namespace Preprocessing;

namespace PreprocessorTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestGrayscaling)
		{
			// TODO: 테스트 코드를 여기에 입력합니다.
			std::shared_ptr<PngImage> img = PngImage::LoadImage(L"C:/Users/JK/Desktop/sample/aa.png");
			img->storeToFile(L"C:/Users/JK/Desktop/sample/aa_copy.png");
			Preprocessor::process(*img);
			img->storeToFile(L"C:/Users/JK/Desktop/sample/aa_gray.png");
		}
	};
}