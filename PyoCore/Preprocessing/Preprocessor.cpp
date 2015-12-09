#include "Preprocessor.h"


namespace Preprocessing 
{
	Preprocessor::Preprocessor()
	{
	}

	Preprocessor::~Preprocessor()
	{
	}

	void Preprocessor::preprocess(PngImage& image)
	{
		applyGrayscale(image);
		applyThreshold(image);
		removeBorder(image);
	}

	void Preprocessor::applyGrayscale(PngImage& colorImage)
	{
		int width = colorImage.getWidth();
		int height = colorImage.getHeight();
		for (int i = 0; i < height; ++i)
		{
			for (int j = 0; j < width; ++j)
			{
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

	void Preprocessor::applyThreshold(PngImage& grayscaledImage)
	{
		int threshold = 128;
		int width = grayscaledImage.getWidth();
		int height = grayscaledImage.getHeight();
		for (int i = 0; i < height; ++i)
		{
			for (int j = 0; j < width; ++j)
			{
				Pixel& pixel = grayscaledImage[i][j];
				unsigned char value = static_cast<unsigned char>(pixel.R > threshold);
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
