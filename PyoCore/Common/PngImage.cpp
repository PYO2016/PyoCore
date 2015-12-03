#include "PngImage.h"
#include "../Lodepng/lodepng.h"

namespace Common
{
	/* PixelArray */

	Pixel& PixelArray::operator[] (int idx)
	{
		return reinterpret_cast<Pixel*>(this)[idx];
	}

	const Pixel& PixelArray::operator[] (int idx) const
	{
		return reinterpret_cast<const Pixel*>(this)[idx];
	}


	/* Image */

	PngImage::PngImage() {}

	PngImage* PngImage::loadImage(const char* filename)	// static member function
	{
		PngImage *image = new PngImage();
		//decode
		//the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA.
		unsigned error = lodepng::decode(image->data, image->width, image->height, filename);

		//if there's an error, display it
		//if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

		if (!error) {
			image->filename = filename;
			return image;
		}
		else {
			delete image;
			return NULL;
		}
	}

	PixelArray& PngImage::operator[] (int idx)
	{
		return *reinterpret_cast<PixelArray*>(&data[idx * width]);
	}

	const PixelArray& PngImage::operator[] (int idx) const
	{
		return *reinterpret_cast<const PixelArray*>(&data[idx * width]);
	}

	std::string PngImage::getFileName() const
	{ return filename; }

	unsigned int PngImage::getWidth() const
	{ return width; }

	unsigned int PngImage::getHeight() const
	{ return height; }

	std::vector<unsigned char>& PngImage::getDataRef()
	{ return data; }

	unsigned char* PngImage::getDataRefAsByteArray()
	{ return &data[0]; }
}