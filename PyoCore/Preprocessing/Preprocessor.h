#pragma once

#include <vector>
#include "../Common/PngImage.h"

using namespace std;
using namespace Common;
using namespace cv;

namespace Preprocessing
{
	class Preprocessor
	{
	public:
		Preprocessor() = delete;
		~Preprocessor() = delete;
		static bool process(PngImage&, int, double);

	private:
		static void removeBorder(Mat&);
		static void applyToOrigin(PngImage&, Mat&);
	};

}