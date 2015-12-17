#pragma once

#include <vector>
#include <string>
#include <memory>

namespace Common
{
#pragma pack(push, 1)
	struct Pixel
	{
		unsigned char R, G, B, A;
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
		struct HiddenKey {};	// Used by LoadImage().

	public:
		inline PngImage(HiddenKey) : PngImage() { };
		// only create image by LoadImage() or copy constructor.
		PngImage(const PngImage& image);
		static std::shared_ptr<PngImage> LoadImage(const std::wstring& filename);

		PixelArray& operator[] (int idx);
		const PixelArray& operator[] (int idx) const;

		bool storeToFile(const std::wstring& targetFilename);

		inline bool PngImage::isCopy() const;
		inline std::wstring PngImage::getFileName() const;
		inline unsigned int PngImage::getWidth() const;
		inline unsigned int PngImage::getHeight() const;
		inline std::vector<unsigned char>& PngImage::getDataRef();
		inline unsigned char* PngImage::getDataRefAsByteArray();

	private:
		bool _isCopy;	// true if created by copy constructor.
		std::wstring filename;
		unsigned int width, height;		// width and height of image.
		std::vector<unsigned char> data;
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
		return width;
	}

	inline unsigned int PngImage::getHeight() const
	{
		return height;
	}

	inline std::vector<unsigned char>& PngImage::getDataRef()
	{
		return data;
	}

	inline unsigned char* PngImage::getDataRefAsByteArray()
	{
		return &data[0];
	}
}


