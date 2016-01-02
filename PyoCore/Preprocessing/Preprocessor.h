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
		static bool process(PngImage&);

	private:
		static void applyToOrigin(PngImage&, Mat&);
	};

}