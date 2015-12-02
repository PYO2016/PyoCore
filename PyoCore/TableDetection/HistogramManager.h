#pragma once

#include "../Common/Image.h"

namespace TableDetection
{
	enum class HistogramType : int
	{
		TYPE_X = 0,
		TYPE_Y
	};

	class Histogram
	{
	public:
		Histogram(HistogramType type, const Common::Image& image, int length, int valLimit);
		Histogram(const Histogram& h);
		~Histogram();

		bool calculateValues();

	private:
		// histogram type ( represents whether x-coordinate or y-coordinate )
		HistogramType type;
		const Common::Image& image;
		int* values;
		int length, valLimit;
	};

	class HistogramManager
	{	
	public:
		HistogramManager(const Common::Image& image);
		HistogramManager(const HistogramManager& h);
		~HistogramManager();

		void cleanup();

		bool makeHistogram(HistogramType type);
		bool applyMedianFilter();	// not yet implemented...
		bool filterExtremum();		// not yet implemented...
		bool applyKmeans();			// not yet implemented...
		/* etc... */

	private:
		const Common::Image& image;
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