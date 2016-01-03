#pragma once

#include "../Common/PngImage.h"

using namespace Common;

namespace TableDetection
{
	class OCRManager
	{
	public:
		OCRManager();
		~OCRManager();
		std::wstring getStringFromRect(PngImage, int, int, int, int);

	private:
	};
}