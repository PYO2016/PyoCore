#pragma once

#include "../Common/PngImage.h"

namespace TableDetection
{
	enum class HistogramType : int
	{
		TYPE_X = 0,
		TYPE_Y
	};
	enum class ExtremumType : int
	{
		TYPE_MIN = 0,
		TYPE_MAX	
	};

	class Histogram
	{
	public:
		Histogram(HistogramType type, const Common::PngImage& image, int length, int valLimit);
		Histogram(const Histogram& h);
		~Histogram();

		bool calculateValues();		// not tested...
		bool applyMedianFilter();	// test pass!
		bool initFilterExtremum();	// not tested

	private:
		// histogram type ( represents whether x-coordinate or y-coordinate )
		HistogramType type;
		const Common::PngImage& image;
		int* values;
		int length, valLimit;
	};

	class HistogramManager
	{	
	public:
		HistogramManager(const Common::PngImage& image);
		HistogramManager(const HistogramManager& h);
		~HistogramManager();

		void cleanup();

		bool makeHistogram(HistogramType type);
		bool applyMedianFilter(HistogramType type);
		bool filterExtremum(HistogramType type);
		//bool applyKmeans();			// not yet implemented...
		/* etc... */

	private:
		const Common::PngImage& image;
		unsigned areaWidth, areaHeight;
		Histogram *histogramX, *histogramY;
	};
}

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

/*
	광민이 해야할 것 (G)---------------
	1. Png To 자료구조	v


	태국이 해야할 것 (T)---------------
	1. makeHistogram (dependency : G-1)	v
*/