#include "PngImage.h"
#include "EncodingConverter.h"

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

	PngImage::PngImage(const PngImage& image)
		:_isCopy(true), filename(image.filename), imageMat(image.imageMat)
	{
	}

	std::shared_ptr<PngImage> PngImage::LoadPngImage(const std::wstring& filename)	// static member function
	{
		std::shared_ptr<PngImage> pImage = std::make_shared<PngImage>(CreationKey());
		pImage->_isCopy = false;
		//decode
		//the pixels are now in the vector "image", 3 bytes per pixel, ordered BGRBGR...
		pImage->imageMat = cv::imread(EncodingConverter::ws2s(filename), cv::IMREAD_COLOR);
		bool error = (pImage->imageMat.data == nullptr);
		if (error)
			return nullptr;

		pImage->filename = filename;
		return pImage;
	}

	PixelArray& PngImage::operator[] (int idx)
	{
		return reinterpret_cast<PixelArray&>(imageMat.data[idx * this->getWidth() * sizeof(Pixel)]);
	}

	const PixelArray& PngImage::operator[] (int idx) const
	{
		return reinterpret_cast<const PixelArray&>(imageMat.data[idx * this->getWidth() * sizeof(Pixel)]);
	}

	bool PngImage::storeToFile(const std::wstring& targetFilename)
	{
		return cv::imwrite(EncodingConverter::ws2s(targetFilename), imageMat);
	}
}