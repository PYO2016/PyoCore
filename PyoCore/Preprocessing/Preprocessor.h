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
		void preprocess(PngImage&);

	private:
		void applyGrayscale(PngImage&);
		void applySimpleThreshold(PngImage&);
		void removeBorder(PngImage&);
	};

}