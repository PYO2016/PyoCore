#pragma once

#include "../Common/PngImage.h"
#include "../../../Tesseract/include/tesseract/baseapi.h"
#include "../../../Leptonica/include/leptonica/allheaders.h"

using namespace Common;
using namespace tesseract;

namespace TableDetection
{
	class OCRManager
	{
	public:
		OCRManager();
		~OCRManager();
		std::wstring getStringFromRect(PngImage, int, int, int, int);

	private:
		TessBaseAPI* tessApi;

	};
}