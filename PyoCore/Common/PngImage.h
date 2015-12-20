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
		struct CreationKey {};	// Used by LoadImage().
		static CreationKey cKey;
	public:
		inline PngImage(CreationKey) : PngImage() { };
		// only create image by LoadImage() or copy constructor.
		PngImage(const PngImage& image);
		PngImage& operator=(const PngImage &) = delete;
		static std::shared_ptr<PngImage> LoadImage(const std::wstring& filename);

		PixelArray& operator[] (int idx);
		const PixelArray& operator[] (int idx) const;

		bool storeToFile(const std::wstring& targetFilename);

		inline bool PngImage::isCopy() const;
		inline std::wstring PngImage::getFileName() const;
		inline unsigned int PngImage::getWidth() const;
		inline unsigned int PngImage::getHeight() const;
		inline std::vector<unsigned char>& PngImage::getData();
		inline const std::vector<unsigned char>& PngImage::getData() const;
		inline unsigned char* PngImage::getDataAsByteArray();
		inline const unsigned char* PngImage::getDataAsByteArray() const;

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
	inline std::vector<unsigned char>& PngImage::getData()
	{
		return data;
	}
	inline const std::vector<unsigned char>& PngImage::getData() const
	{
		return data;
	}
	inline unsigned char* PngImage::getDataAsByteArray()
	{
		return &data[0];
	}
	inline const unsigned char* PngImage::getDataAsByteArray() const
	{
		return &data[0];
	}
}


