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
		// only create image by LoadImage().
		PngImage();
		PngImage(const PngImage& image);

	public:
		// only create image by LoadImage().
		static PngImage* LoadImage(const char *filename);

		PixelArray& operator[] (int idx);
		const PixelArray& operator[] (int idx) const;

		std::string getFileName() const;
		unsigned int getWidth() const;
		unsigned int getHeight() const;
		std::vector<unsigned char>& getDataRef();
		unsigned char* getDataRefAsByteArray();

	private:
		std::string filename;
		unsigned int width, height;		// width and height of image.
		std::vector<unsigned char> data;
	};
}

