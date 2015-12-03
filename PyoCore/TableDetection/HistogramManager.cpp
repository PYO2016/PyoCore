#include "HistogramManager.h"

#include <iostream>
#include <vector>
#include <utility>
#include <functional>
#include <algorithm>
#include "../Lodepng/lodepng.h"

namespace TableDetection
{

	/* Histogram */

	Histogram::Histogram(HistogramType type, const Common::PngImage& image, int length, int valLimit)
		:type(type), image(image), length(length), valLimit(valLimit)
	{
		values = new int[length];
	}

	Histogram::Histogram(const Histogram& h)
		:type(h.type), image(h.image), length(h.length), valLimit(h.valLimit)
	{
		for (int i = 0; i < length; ++i)
			values[i] = h.values[i];
	}

	Histogram::~Histogram()
	{
		delete[] values;
	}

	bool Histogram::calculateValues()
	{
		for (int i = 0; i < length; ++i) {
			values[i] = 0;
			for (int j = 0; j < valLimit; ++j) {
				int v;
				switch (type)
				{
				case HistogramType::TYPE_X: 
					v = image[j][i].R;
					break;
				case HistogramType::TYPE_Y:
					v = image[i][j].R;
					break;
				default:
					return false;	// return.
				}
				if (v == 0) ++values[i];
			}
		}

		return true;
	}

	bool Histogram::applyMedianFilter()
	{
		const int range = 5;	// must be odd number.
		const int halfRange = range / 2;
		std::vector<std::pair<int, int> > valVector;	// val, idx

		if (length < halfRange || range < 3) {
			// nothing to do for median filter.
			return true;	// return value is true or false?? 
		}

		for (int i = 0; i < halfRange; ++i) {
			valVector.push_back(std::make_pair(values[i], i));
		}

		for (int i = 0; i < length; ++i) {
			// In case of right corner of histogram,
			if (i + halfRange >= length) {
				auto iter = std::find_if(valVector.begin(), valVector.end(),
					[i, halfRange](const std::pair<int, int>& v) {
					return v.second == i - halfRange - 1;
				}
				);
				valVector.erase(iter);
			}
			// In case of most positions of histogram,
			else if (valVector.size() == range) {
				auto iter = std::find_if(valVector.begin(), valVector.end(),
					[i, halfRange](const std::pair<int, int>& v) {
					return v.second == i - halfRange - 1;
				}
				);
				*iter = std::make_pair(values[i + halfRange], i + halfRange);
			}
			// In case of left corner of histogram,
			else {
				valVector.push_back(std::make_pair(values[i + halfRange], i + halfRange));
			}

			std::sort(valVector.begin(), valVector.end());
			values[i] = valVector[valVector.size() / 2].first;
		}

		return true;
	}


	/* HistogramManager */

	HistogramManager::HistogramManager(const Common::PngImage& image)
		:image(image), areaWidth(image.getWidth()), areaHeight(image.getHeight()),
			histogramX(NULL), histogramY(NULL)
	{
	}

	HistogramManager::HistogramManager(const HistogramManager& h)
		: image(h.image), areaWidth(h.image.getWidth()), areaHeight(h.image.getHeight()),
		histogramX(h.histogramX), histogramY(h.histogramY)
	{
	}

	HistogramManager::~HistogramManager()
	{
		cleanup();
	}

	void HistogramManager::cleanup()
	{
		if (this->histogramX != NULL) {
			delete histogramX;
			histogramX = NULL;
		}
		if (this->histogramY != NULL) {
			delete histogramY;
			histogramY = NULL;
		}
	}

	bool HistogramManager::makeHistogram(HistogramType type)
	{
		bool success = false;

		switch (type)
		{
		case HistogramType::TYPE_X:
			histogramX = new Histogram(type, image, areaWidth, areaHeight);
			if (!(success = histogramX->calculateValues()))
				delete histogramX;
			break;
			
		case HistogramType::TYPE_Y:
			histogramY = new Histogram(type,image,  areaHeight, areaWidth);
			if (!(success = histogramY->calculateValues()))
				delete histogramY;
			break;
		}

		return success;
	}

	bool HistogramManager::applyMedianFilter(HistogramType type)
	{
		bool success;

		switch (type)
		{
		case HistogramType::TYPE_X:
			success = histogramX->applyMedianFilter();
			break;

		case HistogramType::TYPE_Y:
			success = histogramY->applyMedianFilter();
			break;
		}

		return success;
	}
}