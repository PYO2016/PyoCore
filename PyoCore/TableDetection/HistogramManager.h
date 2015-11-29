#pragma once

namespace TableDetection
{
	enum class HistogramType : int
	{
		TYPE_X = 0,
		TYPE_Y
	};

	class Histogram
	{
		/*
		Attributes List...
			type		// histogram type ( represents whether x-coordinate or y-coordinate )
			...etc...

		Method List...
			...etc...
		*/
		
	};

	class HistogramManager
	{
	private:
		//the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA.
		std::vector<unsigned char> image; //the raw pixels
		unsigned areaWidth, areaHeight;
		/*
		Attributes List...
			areaWidth		// width of area
			areaHeight		// height of area
			histogramX		// histogram to x-coordinate
			histogramY		// histogram to y-coordinate
			...etc...

		Method List...
			initialize(const char*)
			makeHistogram
			applyMedianFilter
			filterExtremum
			applyKmeans
			...etc...
		*/
	public:
		bool initialize(const char*);
	};
}

/*
	광민이 해야할 것 (G)---------------
	1. Png To 자료구조


	태국이 해야할 것 (T)---------------
	1. makeHistogram (dependency : G-1)
*/