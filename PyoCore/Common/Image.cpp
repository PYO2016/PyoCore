#include "Image.h"
#include "../Lodepng/lodepng.h"

namespace Common
{
	/* PixelArray */

	Pixel& PixelArray::operator[] (int idx)
	{
		return ((Pixel*)this)[idx];
	}

	const Pixel& PixelArray::operator[] (int idx) const
	{
		return ((const Pixel*)this)[idx];
	}


	/* Image */

	Image::Image() {}

	Image* Image::loadImage(const char* filename)	// static member function
	{
		Image *image = new Image();
		//decode
		//the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA.
		unsigned error = lodepng::decode(image->data, image->width, image->height, filename);

		//if there's an error, display it
		//if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

		image->filename = filename;

		return error ? NULL : image;
	}

	PixelArray& Image::operator[] (int idx)
	{
		return *(PixelArray*)data[idx * width];
	}

	const PixelArray& Image::operator[] (int idx) const
	{
		return *(const PixelArray*)data[idx * width];
	}

	std::string Image::getFileName() const
	{ return filename; }

	unsigned int Image::getWidth() const
	{ return width; }

	unsigned int Image::getHeight() const
	{ return height; }

	std::vector<unsigned char>& Image::getDataRef()
	{ return data; }

	unsigned char* Image::getDataRefAsByteArray()
	{ return &data[0]; }
}