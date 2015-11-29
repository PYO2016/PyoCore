#include "HistogramManager.h"

#include <iostream>
#include <vector>
#include "lodepng\lodepng.h"

bool TableDetection::HistogramManager::initialize(const char* filename)
{
	//decode
	unsigned error = lodepng::decode(image, areaWidth, areaHeight, filename);

	//if there's an error, display it
	if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

	//the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...
}