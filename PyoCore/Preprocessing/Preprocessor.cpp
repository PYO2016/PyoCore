#include "Preprocessor.h"


namespace Preprocessing 
{
	bool Preprocessor::process(PngImage& image)
	{
		applyGrayscale(image);
		//lowPassFilter(image);
		applySimpleThreshold(image);
		//removeBorder(image);
		return true;
	}

	void Preprocessor::lowPassFilter(PngImage& image)
	{
		const int dir[5][2] = { { 1, 0 },{ 0, 1 },{ -1, 0 },{ 0, -1 },{ 0,0 } };
		PngImage copiedImage(image);
		int width = image.getWidth();
		int height = image.getHeight();

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				int r = 0;
				for (int p = 0; p < 5; p++) {
					int ty = dir[p][0] + i;
					int tx = dir[p][1] + j;
					if (ty < 0)
						ty = 0;
					else if (ty >= height)
						ty = height - 1;
					if (tx < 0)
						tx = 0;
					else if (tx >= width)
						tx = width - 1;
					r += copiedImage[ty][tx].R;
				}
				image[i][j].R = r/5;
			}
		}
		return;
	}

	void Preprocessor::applyGrayscale(PngImage& colorImage)
	{
		int width = colorImage.getWidth();
		int height = colorImage.getHeight();
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) {
				Pixel& pixel = colorImage[i][j];

				unsigned char r = pixel.R;
				unsigned char g = pixel.G;
				unsigned char b = pixel.B;
				unsigned char gray = static_cast<unsigned char>(0.2989 * r + 0.5870 * g + 0.1140 * b);
				pixel.R = gray;
				pixel.G = gray;
				pixel.B = gray;
			}
		}
	}

	void Preprocessor::applySimpleThreshold(PngImage& grayImage, int threshold)
	{
		int width = grayImage.getWidth();
		int height = grayImage.getHeight();
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) {
				Pixel& pixel = grayImage[i][j];
				unsigned char value = (pixel.R > threshold ? 255 : 0);
				pixel.R = value;
				pixel.G = value;
				pixel.B = value;
			}
		}
	}

	void Preprocessor::removeBorder(PngImage& image)
	{
	}
}
