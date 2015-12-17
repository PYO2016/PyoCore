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
		Preprocessor();
		~Preprocessor();
		void process(PngImage&);

	private:
		static const int DEFAULT_THRESHOLD = 128;

		void applyGrayscale(PngImage&);
		void applySimpleThreshold(PngImage&, int = DEFAULT_THRESHOLD);
		void removeBorder(PngImage&);
	};

}