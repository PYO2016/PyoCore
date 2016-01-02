#include "OCRManager.h"
#include <string>

#pragma comment(lib, "C:/Users/KGWANGMIN/git/PYO2016/Leptonica/lib/liblept168")
#pragma comment(lib, "C:/Users/KGWANGMIN/git/PYO2016/Tesseract/lib/libtesseract302")


namespace TableDetection
{
	OCRManager::OCRManager()
	{
		tessApi = new TessBaseAPI();
		if (tessApi == NULL || tessApi->Init(NULL, "eng")) {
			return;
		}
	}
	
	OCRManager::~OCRManager()
	{
		if (tessApi != NULL)
			tessApi->End();
	}

	std::wstring OCRManager::getStringFromRect(PngImage image, int left, int top, int width, int height)
	{
		unsigned char* imagedata = image.getDataAsByteArray();
		int depth = 1;
		int rowbytes = width * depth * 4;

		tessApi->SetImage(imagedata, width, height, depth, rowbytes);
		tessApi->SetRectangle(left, top, width, height);

		std::string str(tessApi->GetUTF8Text());
		return std::wstring(str.begin(), str.end());
	}
}