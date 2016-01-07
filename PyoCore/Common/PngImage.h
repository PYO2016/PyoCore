#pragma once

#include <vector>
#include <string>
#include <memory>
#include "opencv2\opencv.hpp"

namespace Common
{
#pragma pack(push, 1)
	struct Pixel
	{
		unsigned char B, G, R;
	};
#pragma pack(pop)

	class PixelArray
	{
	public:
		Pixel& operator[] (int idx);
		const Pixel& operator[] (int idx) const;
	};

	class PngImage
	{
	private:
		PngImage() = default;
		struct CreationKey {};
	public:
		PngImage(CreationKey) : PngImage() { };
		// only create image by LoadImage() or copy constructor.
		PngImage(const PngImage& image);
		PngImage& operator=(const PngImage &) = delete;
		static std::shared_ptr<PngImage> LoadPngImage(const std::wstring& filename);

		PixelArray& operator[] (int idx);
		const PixelArray& operator[] (int idx) const;

		bool storeToFile(const std::wstring& targetFilename);

		inline bool PngImage::isCopy() const;
		inline std::wstring PngImage::getFileName() const;
		inline unsigned int PngImage::getWidth() const;
		inline unsigned int PngImage::getHeight() const;
		inline unsigned char* PngImage::getDataAsByteArray();
		inline const unsigned char* PngImage::getDataAsByteArray() const;
		inline void setPadding(int);

	private:
		bool _isCopy;	// true if created by copy constructor.
		std::wstring filename;
		cv::Mat imageMat;
	};

	/* inline functions */

	inline bool PngImage::isCopy() const
	{
		return _isCopy;
	}
	inline std::wstring PngImage::getFileName() const
	{
		return filename;
	}
	inline unsigned int PngImage::getWidth() const
	{
		return imageMat.cols;
	}
	inline unsigned int PngImage::getHeight() const
	{
		return imageMat.rows;
	}
	inline unsigned char* PngImage::getDataAsByteArray()
	{
		return imageMat.data;
	}
	inline const unsigned char* PngImage::getDataAsByteArray() const
	{
		return imageMat.data;
	}

	inline void PngImage::setPadding(int padding) {
		cv::copyMakeBorder(this->imageMat, this->imageMat, padding, padding, padding, padding, cv::BORDER_CONSTANT, cv::Scalar::all(255));
	}
}


