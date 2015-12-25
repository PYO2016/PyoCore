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
			values(length), length(length), valLimit(valLimit), extremumList(0)
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
		auto eListMaxV = values[0], eListMinV = values[0];
		if (length < 2) 
		{
			//I think it can not process
			return true;
		}
		currentState = ((values[0] - values[1]) > 0) ? ExtremumType::TYPE_MAX: ExtremumType::TYPE_MIN;
		eList.emplace_back(0, currentState);
		for (int i = 2; i < length; ++i)
		{
			if (values[i] - values[i - 1] == 0)
			{
				continue;
			}
			nextState = ((values[i - 1] - values[i]) > 0) ? ExtremumType::TYPE_MAX : ExtremumType::TYPE_MIN;
			if (currentState != nextState)
			{
				//state changed
				if (eListMaxV < values[i - 1])
				{
					eListMaxV = values[i - 1];
				}
				if (eListMinV > values[i - 1] && values[i - 1] != 0)
				{
					eListMinV = values[i - 1];
				}
				eList.emplace_back(i - 1, nextState);
				currentState = nextState;
			}
		}
		if (values[length - 1] != values[length - 2])
		{
			eList.emplace_back(length - 1, ((values[length - 1] - values[length - 2]) > 0) ? ExtremumType::TYPE_MAX : ExtremumType::TYPE_MIN);
		}

		// remove non-reasonable value
		eListMaxV = static_cast<int>(ceil(static_cast<double>(eListMaxV) * 0.8));
		//eListMinV = static_cast<int>(ceil(static_cast<double>(eListMinV) * 1.2));
		for (auto currItr = begin(eList); currItr != end(eList); ++currItr)
		{
			if (currItr->second == ExtremumType::TYPE_MAX && values[currItr->first] < eListMaxV)
			{
				currItr = eList.erase(currItr);
				if(currItr != begin(eList))
					--currItr;
			}
			//else if (currItr->second == ExtremumType::TYPE_MIN && values[currItr->first] > eListMinV)
			//{
			//	currItr = eList.erase(currItr);
			//	if(currItr != begin(eList))
			//		--currItr;
			//}
			//if (abs(values[nextItr->first] - values[currItr->first]) < eListMaxV)
			//{
			//	eList.erase(nextItr);
			//}
		}
		// data align
		for (auto currItr = begin(eList); currItr != end(eList); ++currItr)
		{
			auto nextItr = next(currItr, 1);
			if (nextItr == end(eList))
				break;
			while (nextItr != end(eList) && currItr->second == nextItr->second)
			{
				if (nextItr != end(eList) && currItr->second == ExtremumType::TYPE_MAX)
					if (values[currItr->first] < values[nextItr->first])
						currItr->first = nextItr->first;
				else
					if (values[nextItr->first] < values[currItr->first])
						currItr->first = nextItr->first;
				eList.erase(nextItr);
				nextItr = next(currItr, 1);
			}
		}
		this->extremumList = std::move(eList);
		return true;
	}

	double Histogram::getKmeansBoundary(ExtremumType type)
	{
		std::vector<int> forCluster;
		for (std::pair<int, ExtremumType> p : this->extremumList)
		{
			if (p.second == type)
				forCluster.emplace_back(p.first);
		}
		std::vector<KmeansType> clustered(forCluster.size());
		// for get 1/4th value, 3/4th value
		std::vector<int> forClusterTemp{ forCluster };

		std::vector<int>& vForSort = this->values;

		std::sort(begin(forClusterTemp), end(forClusterTemp), 
			[&vForSort](auto a, auto b)
		{
			return vForSort[a] < vForSort[b];
		});

		double lower, upper;

		lower = static_cast<double>(this->values[forClusterTemp[0]]);
		upper = static_cast<double>(this->values[forClusterTemp[forClusterTemp.size()-1]]);

		// actually this while loop must divide as 2 group(xCluster, yCluser) but... just my tiresome
		while (true)
		{
			double currentLow = 0, currentUpper = 0;
			int lowCnt = 0, upperCnt = 0;
			for (int i = 0; i < forCluster.size(); i++)
			{
				clustered[i] = (abs(lower - this->values[forCluster[i]]) > abs(upper - this->values[forCluster[i]])) ? KmeansType::TYPE_UPPER : KmeansType::TYPE_LOWER;
				// if clustered as lower
				if (clustered[i] == KmeansType::TYPE_LOWER)
				{
					currentLow += this->values[forCluster[i]];
					++lowCnt;
				}
				else
				{
					currentUpper += this->values[forCluster[i]];
					++upperCnt;
				}
			}
			currentLow /= lowCnt;
			currentUpper /= upperCnt;
			if (lowCnt == 0 || upperCnt == 0)
			{
				return 0;
			}
			if (lower == currentLow &&
				upper == currentUpper)
			{
				// can compiler optimize this while loop?
				// if cant i will modify this code as do-while
				break;
			}
			lower = currentLow;
			upper = currentUpper;
		}
		int lowerMaxValue = INT_MIN;
		int upperMinValue = INT_MAX;
		for (int i = 0; i < clustered.size(); i++)
		{
			if (clustered[i] == KmeansType::TYPE_LOWER && this->values[forCluster[i]] > lowerMaxValue)
				lowerMaxValue = this->values[forCluster[i]];
			else if (clustered[i] == KmeansType::TYPE_UPPER && this->values[forCluster[i]] < upperMinValue)
				upperMinValue = this->values[forCluster[i]];
		}
		
		return ((static_cast<double>(lowerMaxValue) + static_cast<double>(upperMinValue)) / 2);
	}
	bool Histogram::removeKmeansValues(double minBoundary, double maxBoundary)
	{
		bool success = true;
		for (auto itr = begin(this->extremumList); itr != end(this->extremumList); ++itr)
		{
			if (itr->second == ExtremumType::TYPE_MAX &&
				itr->first > maxBoundary)
			{
				auto jtr = next(itr);
				for (; jtr != end(this->extremumList) && jtr->second != ExtremumType::TYPE_MAX && jtr->first < maxBoundary; ++jtr);

				if (jtr == end(this->extremumList))
				{
					goto outerLoop;
				}
				else 
				{
					int minValue = INT_MAX;
					for (auto ktr = next(itr); ktr != jtr; ++ktr)
					{
						if (minValue > ktr->first && ktr->second == ExtremumType::TYPE_MIN && ktr->first < minBoundary)
						{
							minValue = ktr->first;
						}
					}
					for (auto ktr = next(itr); ktr != jtr; ++ktr)
					{
						if (minValue > ktr->first && ktr->second == ExtremumType::TYPE_MIN && ktr->first < minBoundary)
						{
							ktr = this->extremumList.erase(ktr);
							--ktr;
						}
					}
				}
			}
		}
		outerLoop:
		return success;
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
	bool HistogramManager::applyKmeans(HistogramType type)
	{
		bool success = false;

		switch (type)
		{
		case HistogramType::TYPE_X:
			success = pHistogramX->removeKmeansValues(pHistogramX->getKmeansBoundary(ExtremumType::TYPE_MIN), pHistogramX->getKmeansBoundary(ExtremumType::TYPE_MAX));
			break;
		case HistogramType::TYPE_Y:
			success = pHistogramY->removeKmeansValues(pHistogramY->getKmeansBoundary(ExtremumType::TYPE_MIN), pHistogramY->getKmeansBoundary(ExtremumType::TYPE_MAX));
			break;
		}

		return success;
	}
	std::vector<std::tuple<int, int, int, int>> HistogramManager::getTableInfo()
	{
		std::vector<std::tuple<int, int, int, int>> tableVector;

		auto xExtremum  = pHistogramX->getExtremumList();
		auto yExtremum  = pHistogramY->getExtremumList();

		int top, bottom, left, right;
		for (auto itr = begin(yExtremum); itr != prev(end(yExtremum)); ++itr)
		{
			if (itr->second == ExtremumType::TYPE_MIN)
				continue;
			top = itr->first;
			bottom = next(itr)->first;
			for (auto jtr = begin(xExtremum); jtr != prev(end(xExtremum)); ++jtr)
			{
				if (jtr->second == ExtremumType::TYPE_MIN)
					continue;
				left = jtr->first;
				right = next(jtr)->first;
				tableVector.emplace_back(top, bottom, left, right);
			}
		}

		return tableVector;
	}
}