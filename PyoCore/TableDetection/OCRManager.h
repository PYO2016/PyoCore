#pragma once

#include "../Common/PngImage.h"
#include "../Common/SparseBlock.h"
#include "../Common/Table.h"
#include <list>
#include <vector>

namespace TableDetection
{
	class OCRManager
	{
	public:
		OCRManager();
		~OCRManager();

		static bool recognize(Common::PngImage&, std::list<Common::SparseBlock>&);
		static bool recognize(Common::PngImage&, std::vector<Common::Cell>&);
		static void localProcess(cv::Mat&);
	};
}