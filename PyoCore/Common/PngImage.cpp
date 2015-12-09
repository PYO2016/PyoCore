#include "PngImage.h"
#include "EncodingConverter.h"
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

	PngImage::PngImage() { }

	PngImage::PngImage(const PngImage& image)
		:isCopy(true), filename(image.filename), width(image.width), height(image.height), 
			data(image.data)
	{
	}

	std::shared_ptr<PngImage> PngImage::LoadImage(const std::wstring& filename)	// static member function
	{
		std::shared_ptr<PngImage> pImage(new PngImage());
		pImage->isCopy = false;
		//decode
		//the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA.
		unsigned error = lodepng::decode(pImage->data, pImage->width, pImage->height,
			EncodingConverter::ws2s(filename));

		//if there's an error, display it
		//if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

		if (!error) {
			pImage->filename = filename;
			return pImage;
		}
		else {
			return nullptr;
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

	bool PngImage::storeToFile(const std::wstring& targetFilename)
	{
		return lodepng::save_file(data, EncodingConverter::ws2s(targetFilename)) == 0 ? true : false;
	}

	bool PngImage::getIsCopy() const
	{ return isCopy; }

	std::wstring PngImage::getFileName() const
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