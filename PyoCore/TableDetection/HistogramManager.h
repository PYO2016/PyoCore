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
		Histogram(HistogramType type, const Common::PngImage& image, 
			unsigned offsetWidth, unsigned offsetHeight, int length, int valLimit);
		Histogram(const Histogram& h);
		~Histogram();

		bool calculateValues();		// not tested...
		bool applyMedianFilter();	// test pass!
		bool initFilterExtremum();	// not tested

	private:
		// histogram type ( represents whether x-coordinate or y-coordinate )
		HistogramType type;
		const Common::PngImage& image;
		unsigned offsetWidth, offsetHeight;
		std::vector<int> values;
		int length, valLimit;
	};

	class HistogramManager
	{	
	public:
		HistogramManager(const Common::PngImage& image);
		HistogramManager(const Common::PngImage& image, unsigned areaWidth, unsigned areaHeight, 
			unsigned offsetWidth, unsigned offsetHeight);
		HistogramManager(const HistogramManager& h);
		~HistogramManager();

		void cleanup();

		bool makeHistogram(HistogramType type);
		bool applyMedianFilter(HistogramType type);
		bool filterExtremum(HistogramType type);
		//bool applyKmeans(HistogramType type);			// not yet implemented...
		// 
		/* etc... */

	private:
		const Common::PngImage& image;
		unsigned areaWidth, areaHeight;
		unsigned offsetWidth, offsetHeight;
		Histogram *histogramX, *histogramY;
	};
}