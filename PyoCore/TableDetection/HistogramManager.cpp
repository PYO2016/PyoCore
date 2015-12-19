#include "HistogramManager.h"

#include <iostream>
#include <vector>
#include <array>
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
		std::array<std::pair<int, int>, range> valArray;	// val, idx
		int cnt = 0;
		
		if (length < halfRange || range < 3) {
			// nothing to do for median filter.
			return true;	// return value is true or false?? 
		}

		for (int i = 0; i < halfRange; ++i) {
			valArray[cnt].first = values[i];
			valArray[cnt++].second = i;
		}

		for (int i = 0; i < length; ++i) {
			// In case of right corner of histogram,
			if (i + halfRange >= length) {
				auto iter = std::find_if(std::begin(valArray), std::begin(valArray) + cnt,
					[i, halfRange](const std::pair<int, int>& v) {
					return v.second == i - halfRange - 1;
				}
				);
				while (iter < std::begin(valArray) + cnt - 1) {
					*iter = *std::next(iter);
					iter = std::next(iter);
				}
				--cnt;
			}
			// In case of most positions of histogram,
			else if (cnt == range) {
				auto iter = std::find_if(std::begin(valArray), std::begin(valArray) + cnt,
					[i, halfRange](const std::pair<int, int>& v) {
					return v.second == i - halfRange - 1;
				}
				);
				(*iter).first = values[i + halfRange];
				(*iter).second = i + halfRange;
			}
			// In case of left corner of histogram,
			else {
				valArray[cnt].first = values[i + halfRange];
				valArray[cnt++].second = i + halfRange;
			}

			std::sort(std::begin(valArray), std::begin(valArray) + cnt);
			values[i] = valArray[cnt / 2].first;
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
}