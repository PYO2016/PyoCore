#pragma once

#include <vector>
#include "../Common/PngImage.h"

using namespace std;
using namespace Common;

namespace Preprocessing
{
	class Preprocessor
	{
	public:
		Preprocessor() = delete;
		~Preprocessor() = delete;
		static bool process(PngImage&);


	private:
		static const int DEFAULT_THRESHOLD = 128;

		static void applyGrayscale(PngImage&);
		static void applySimpleThreshold(PngImage&, int = DEFAULT_THRESHOLD);
		static void lowPassFilter(PngImage & image);
		static void removeBorder(PngImage&);
	};

}