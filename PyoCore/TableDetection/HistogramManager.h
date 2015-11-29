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

/*
	광민이 해야할 것 (G)---------------
	1. Png To 자료구조


	태국이 해야할 것 (T)---------------
	1. makeHistogram (dependency : G-1)
*/