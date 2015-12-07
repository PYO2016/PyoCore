#pragma once

#include <vector>
#include <string>

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
		PngImage();

	public:
		// only create image by LoadImage() or copy constructor.
		PngImage(const PngImage& image);
		static PngImage* LoadImage(const std::wstring& filename);

		PixelArray& operator[] (int idx);
		const PixelArray& operator[] (int idx) const;

		bool storeToFile(const std::wstring& targetFilename);

		bool getIsCopy() const;
		std::wstring getFileName() const;
		unsigned int getWidth() const;
		unsigned int getHeight() const;
		std::vector<unsigned char>& getDataRef();
		unsigned char* getDataRefAsByteArray();

	private:
		bool isCopy;	// true if created by copy constructor.
		std::wstring filename;
		unsigned int width, height;		// width and height of image.
		std::vector<unsigned char> data;
	};
}

