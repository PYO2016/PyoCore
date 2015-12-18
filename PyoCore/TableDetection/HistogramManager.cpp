#include "HistogramManager.h"

#include <iostream>
#include <vector>
#include <list>
#include <utility>
#include <functional>
#include <algorithm>
#include "../Lodepng/lodepng.h"

namespace TableDetection
{

	/* Histogram */

	Histogram::Histogram(HistogramType type, const Common::PngImage& image, 
		unsigned offsetWidth, unsigned offsetHeight, int length, int valLimit)
		: type(type), image(image), offsetWidth(offsetWidth), offsetHeight(offsetHeight), 
			values(length), length(length), valLimit(valLimit)
	{
	}

	Histogram::Histogram(const Histogram& h)
		: type(h.type), image(h.image), length(h.length), valLimit(h.valLimit)
	{
		for (int i = 0; i < length; ++i)
			values[i] = h.values[i];
	}

	Histogram::~Histogram()
	{
	}

	bool Histogram::calculateValues()
	{
		const unsigned& ow = offsetWidth;
		const unsigned& oh = offsetHeight;

		for (int i = 0; i < length; ++i) {
			values[i] = 0;
			for (int j = 0; j < valLimit; ++j) {
				int v;
				switch (type)
				{
				case HistogramType::TYPE_X: 
					v = image[ow + j][oh + i].R;
					break;
				case HistogramType::TYPE_Y:
					v = image[ow + i][oh + j].R;
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
			valVector.emplace_back(values[i], i);
		}

		for (int i = 0; i < length; ++i) {
			// In case of right corner of histogram,
			if (i + halfRange >= length) {
				auto iter = std::find_if(begin(valVector), end(valVector),
					[i, halfRange](const std::pair<int, int>& v) {
					return v.second == i - halfRange - 1;
				}
				);
				valVector.erase(iter);
			}
			// In case of most positions of histogram,
			else if (valVector.size() == range) {
				auto iter = std::find_if(begin(valVector), end(valVector),
					[i, halfRange](const std::pair<int, int>& v) {
					return v.second == i - halfRange - 1;
				}
				);
				*iter = std::make_pair(values[i + halfRange], i + halfRange);
			}
			// In case of left corner of histogram,
			else {
				valVector.emplace_back(values[i + halfRange], i + halfRange);
			}

			std::sort(begin(valVector), end(valVector));
			values[i] = valVector[valVector.size() / 2].first;
		}

		return true;
	}

	bool Histogram::initFilterExtremum()
	{
		// find minmax value
		std::list<std::pair<int, ExtremumType>> eList;
		TableDetection::ExtremumType currentState, nextState;
		auto eListMaxV = values[0];
		if (length < 2) 
		{
			//I think it can not process
			return true;
		}
		currentState = ((values[1] - values[0]) > 0) ? ExtremumType::TYPE_MAX: ExtremumType::TYPE_MIN;
		for (int i = 2; i < length; ++i)
		{
			if (values[i] - values[i - 1] == 0)
			{
				continue;
			}
			nextState = ((values[i] - values[i - 1]) > 0) ? ExtremumType::TYPE_MAX : ExtremumType::TYPE_MIN;
			if (currentState != nextState)
			{
				//state changed
				if (eListMaxV < values[i - 1])
				{
					eListMaxV = values[i - 1];
				}
				eList.emplace_back(i - 1, currentState);
				currentState = nextState;
			}
		}

		// remove non-reasonable value
		eListMaxV = static_cast<int>(ceil(static_cast<double>(eListMaxV) * 0.2));
		for (auto currItr = begin(eList); currItr != end(eList); ++currItr)
		{
			auto nextItr = next(currItr, 1);
			if (nextItr == end(eList))
				continue;
			if (abs(values[nextItr->first] - values[currItr->first]) < eListMaxV)
			{
				eList.erase(nextItr);
			}
		}
		// data align
		for (auto currItr = begin(eList); currItr != end(eList); ++currItr)
		{
			auto nextItr = next(currItr, 1);
			if (nextItr == end(eList))
				continue;
			while (currItr->second == nextItr->second)
			{
				if (currItr->second == ExtremumType::TYPE_MAX)
					if (values[currItr->first] < values[nextItr->first])
						currItr->first = nextItr->first;
				else
					if (values[nextItr->first] < values[currItr->first])
						currItr->first = nextItr->first;
				eList.erase(nextItr);
				nextItr = next(currItr, 1);
			}
		}
		return true;
	}

	std::vector<int> Histogram::getValues()
	{
		return this->values;
	}


	/* HistogramManager */

	HistogramManager::HistogramManager(const Common::PngImage& image)
		: HistogramManager(image, image.getWidth(), image.getHeight(), 0, 0)
	{
	}

	HistogramManager::HistogramManager(const Common::PngImage& image,
		unsigned areaWidth, unsigned areaHeight, unsigned offsetWidth, unsigned offsetHeight)
		: image(image), areaWidth(areaWidth), areaHeight(areaHeight), 
			offsetWidth(offsetWidth), offsetHeight(offsetHeight), pHistogramX(nullptr), pHistogramY(nullptr)
	{
	}

	HistogramManager::HistogramManager(const HistogramManager& h)
		: HistogramManager(h.image, h.areaWidth, h.areaHeight, h.offsetWidth, h.offsetHeight)
	{
	}

	HistogramManager::~HistogramManager()
	{
		cleanup();
	}

	void HistogramManager::cleanup()
	{
		pHistogramX.reset();
		pHistogramY.reset();
	}

	bool HistogramManager::makeHistogram(HistogramType type)
	{
		bool success = false;

		switch (type)
		{
		case HistogramType::TYPE_X:
			pHistogramX = std::make_shared<Histogram>(type, image, offsetWidth, offsetHeight, areaWidth, areaHeight);
			if (pHistogramX && !(success = pHistogramX->calculateValues()))
				pHistogramX.reset();
			break;
			
		case HistogramType::TYPE_Y:
			pHistogramY = std::make_shared<Histogram>(type, image, offsetWidth, offsetHeight, areaHeight, areaWidth);
			if (pHistogramY && !(success = pHistogramY->calculateValues()))
				pHistogramY.reset();
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
			success = pHistogramX->applyMedianFilter();
			break;

		case HistogramType::TYPE_Y:
			success = pHistogramY->applyMedianFilter();
			break;
		}

		return success;
	}
	bool HistogramManager::filterExtremum(HistogramType type)
	{
		bool success;

		switch (type)
		{
		case HistogramType::TYPE_X:
			success = pHistogramX->initFilterExtremum();
			break;

		case HistogramType::TYPE_Y:
			success = pHistogramY->initFilterExtremum();
			break;
		}

		return success;
	}
	bool HistogramManager::applyKmeans()
	{
		bool success = false;

		bool LOWER = false;
		bool UPPER = true;
		double xLower, xUpper, yLower, yUpper;
		std::vector<int> x{ pHistogramX->getValues() };
		std::vector<int> y{ pHistogramY->getValues() };
		// for get 1/4th value, 3/4th value
		std::vector<int> tempX{ pHistogramX->getValues() };
		std::vector<int> tempY{ pHistogramY->getValues() };
		std::vector<bool> xClustered((tempX.size()));
		std::vector<bool> yClustered((tempY.size()));

		std::sort(begin(tempX), end(tempX));
		std::sort(begin(tempY), end(tempY));

		xLower = static_cast<double>(tempX[(tempX.size() - 1) / 4]);
		xUpper = static_cast<double>(tempX[((tempX.size() - 1) / 4) * 3]);

		yLower = static_cast<double>(tempX[(tempY.size() - 1) / 4]);
		yUpper = static_cast<double>(tempX[((tempY.size() - 1) / 4) * 3]);

		while (true)
		{
			double currentXLow = 0, currentXUpper = 0;
			double currentYLow = 0, currentYUpper = 0;
			for (int i = 0; i < xClustered.size(); i++)
			{
				xClustered[i] = (abs(xLower - x[i]) > abs(xUpper - x[i])) ? UPPER : LOWER;
				// if clustered as lower
				if (xClustered[i] == LOWER)
				{
					currentXLow += x[i];
				}
				else
				{
					currentXUpper += x[i];
				}
			}
			currentXLow /= xClustered.size();
			currentXUpper /= xClustered.size();
			for (int i = 0; i < yClustered.size(); i++)
			{
				yClustered[i] = (abs(yLower - x[i]) > abs(yUpper - x[i])) ? UPPER : LOWER;
				// if clustered as lower
				if (yClustered[i] == LOWER)
				{
					currentXLow += y[i];
				}
				else
				{
					currentXUpper += y[i];
				}
			}
			yMid /= yClustered.size();
		}
		return success;
	}
}