#include "PngImage.h"
#include "EncodingConverter.h"
#include <vector>

using namespace cv;

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
		:_isCopy(true), filename(image.filename), imageMat(image.imageMat.clone())
	{
	}

	std::shared_ptr<PngImage> PngImage::LoadPngImage(const std::wstring& filename)	// static member function
	{
		std::shared_ptr<PngImage> pImage = std::make_shared<PngImage>(CreationKey());
		pImage->_isCopy = false;
		//decode
		//the pixels are now in the vector "image", 3 bytes per pixel, ordered BGRBGR...
		Mat src = imread(EncodingConverter::ws2s(filename), IMREAD_UNCHANGED);
		src.convertTo(src, CV_8UC4);

		Mat channels[4];
		split(src.clone(), channels);
		
		std::vector<Mat> newChannels(3);
		for (size_t i = 0; i < 3; i++) {
			newChannels[i] = Mat(src.rows, src.cols, CV_8UC1, Scalar(255));
			channels[i].copyTo(newChannels[i], channels[3]);
		}
		merge(newChannels, pImage->imageMat);

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