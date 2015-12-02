#include "HistogramManager.h"

#include <iostream>
#include <vector>
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
}