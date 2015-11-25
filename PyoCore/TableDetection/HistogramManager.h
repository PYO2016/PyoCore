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
		/*
		Attributes List...
			areaWidth		// width of area
			areaHeight		// height of area
			histogramX		// histogram to x-coordinate
			histogramY		// histogram to y-coordinate
			...etc...

		Method List...
			initialize
			makeHistogram
			applyMedianFilter
			filterExtremum
			applyKmeans
			...etc...
		*/
	};
}