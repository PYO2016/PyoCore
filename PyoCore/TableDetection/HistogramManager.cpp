#include "HistogramManager.h"

#include <iostream>
#include <vector>
#include <array>
#include <list>
#include <utility>
#include <functional>
#include <algorithm>
#include <cmath>

namespace TableDetection
{

	/* Histogram */

	Histogram::Histogram(HistogramType type, const Common::PngImage& image,
		unsigned offsetWidth, unsigned offsetHeight, int length, int valLimit, bool edgeExist)
		: type(type), image(image), offsetWidth(offsetWidth), offsetHeight(offsetHeight),
			values(length), length(length), valLimit(valLimit), extremumList(0), edgeExist(edgeExist)
	{
	}

	Histogram::Histogram(const Histogram& h)
		: type(h.type), image(h.image), length(h.length), valLimit(h.valLimit),
			extremumList(h.extremumList), edgeExist(h.edgeExist), visibleLines(h.visibleLines)
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
					v = image[oh + j][ow + i].R;
					break;
				case HistogramType::TYPE_Y:
					v = image[oh + i][ow + j].R;
					break;
				default:
					return false;	// return.
				}
				if (v == 0) ++values[i];
			}
		}

		return true;
	}

	bool Histogram::detectVisibleLines()
	{
		int maxVal = INT_MIN;
		int minVal = INT_MAX;
		for (int i = 0; i < length; ++i) {
			if (values[i] > maxVal)
				maxVal = values[i];
			if (values[i] < maxVal)
				minVal = values[i];
		}
		
		// * TODO : select middle things.
		for (int i = 0; i < length; ++i) {
			if (values[i] > maxVal * 0.9 ) {
				this->visibleLines.emplace_back(i, ExtremumType::TYPE_MAX);
			}
			else if (values[i] < minVal * 1.1) {
				this->visibleLines.emplace_back(i, ExtremumType::TYPE_MIN);
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
		int i;
		auto& eList = this->extremumList;
		TableDetection::ExtremumType prevState, curState;
		int prevIdx, notEqualIdx;
		auto eListMaxV = values[0];
		if (length < 2) 
		{
			//I think it can not process
			return true;
		}
		prevState = ExtremumType::NOTHING;
		prevIdx = -1;		// last previous extremum idx.
		notEqualIdx = 0;	// last idx where values[idx-1] != values[idx].
		for (i = 0; i < length - 1; ++i)
		{
			if (values[i] == values[i + 1])
			{
				continue;
			}
			notEqualIdx = i + 1;
			curState = (values[i] > values[i + 1]) ? ExtremumType::TYPE_MAX : ExtremumType::TYPE_MIN;
			if (prevState != curState)
			{
				//state changed
				if (eListMaxV < values[i])
				{
					eListMaxV = values[i];
				}
				eList.emplace_back(i, curState);
				prevState = curState;
				prevIdx = i;
			}
		}
		if (values[length - 2] > values[length - 1])
		{
			if (prevState == ExtremumType::TYPE_MAX)
				eList.emplace_back(length - 1, ExtremumType::TYPE_MIN);
		}
		else if (values[length - 2] < values[length - 1])
		{
			if (prevState == ExtremumType::TYPE_MIN)
				eList.emplace_back(length - 1, ExtremumType::TYPE_MAX);
		}
		else
		{
			if (prevIdx >= 0) 
			{
				// Always, values[prevIdx] != values[notEqualIdx]
				eList.emplace_back(notEqualIdx, (values[prevIdx] < values[notEqualIdx] ?
					ExtremumType::TYPE_MAX : ExtremumType::TYPE_MIN));
			}
		}
		
		if (this->edgeExist)
		{
			if (eList.empty() || eList.front().first > 0)
				eList.emplace_front(0, ExtremumType::TYPE_MIN);
			if (eList.empty() || eList.back().first < length - 1)
				eList.emplace_back(length - 1, ExtremumType::TYPE_MIN);
		}
		
		// remove non-reasonable value
		eListMaxV = static_cast<int>(std::ceil(static_cast<double>(eListMaxV) * 0.2));
		for (auto currItr = begin(eList); currItr != end(eList); )
		{
			auto nextItr = next(currItr, 1);
			if (nextItr == end(eList))
				break;
			if (abs(values[nextItr->first] - values[currItr->first]) < eListMaxV)
				eList.erase(nextItr);
			else
				++currItr;
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
				{
					if (values[currItr->first] < values[nextItr->first])
					{
						currItr->first = nextItr->first;
					}
				}
				else if (nextItr != end(eList) && currItr->second == ExtremumType::TYPE_MIN)
				{
					if (values[nextItr->first] < values[currItr->first])
					{
						currItr->first = nextItr->first;
					}
				}
				eList.erase(nextItr);
				nextItr = next(currItr, 1);
			}
		}
		return true;
	}

	double Histogram::getKmeansBoundary(ExtremumType type)
	{
		std::vector<int> forCluster;
		for (const std::pair<int, ExtremumType>& p : this->extremumList)
		{
			if (p.second == type)
				forCluster.emplace_back(p.first);
		}

		if (forCluster.empty())
		{
			// no meaning return value.
			return 0;
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

		double currentLow = static_cast<double>(this->values[forClusterTemp[0]]);
		double currentUpper = static_cast<double>(this->values[forClusterTemp[forClusterTemp.size()-1]]);
		double lower, upper;

		// actually this while loop must divide as 2 group(xCluster, yCluser) but... just my tiresome
		do
		{
			int lowCnt = 0, upperCnt = 0;
			lower = currentLow;
			upper = currentUpper;
			currentUpper = currentLow = 0;
			for (int i = 0; i < forCluster.size(); i++)
			{
				if (type == ExtremumType::TYPE_MAX)
					clustered[i] = (abs(lower - this->values[forCluster[i]]) >= abs(upper - this->values[forCluster[i]])) ?
						KmeansType::TYPE_UPPER : KmeansType::TYPE_LOWER;
				else
					clustered[i] = (abs(lower - this->values[forCluster[i]]) > abs(upper - this->values[forCluster[i]])) ?
						KmeansType::TYPE_UPPER : KmeansType::TYPE_LOWER;
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
			if (lowCnt > 0) currentLow /= lowCnt;
			if (upperCnt > 0) currentUpper /= upperCnt;
		}while (lower != currentLow || upper != currentUpper);

		int lowerMaxValue = INT_MIN;
		int upperMinValue = INT_MAX;
		for (int i = 0; i < clustered.size(); i++)
		{
			if (clustered[i] == KmeansType::TYPE_LOWER && this->values[forCluster[i]] > lowerMaxValue)
				lowerMaxValue = this->values[forCluster[i]];
			else if (clustered[i] == KmeansType::TYPE_UPPER && this->values[forCluster[i]] < upperMinValue)
				upperMinValue = this->values[forCluster[i]];
		}
		if (lowerMaxValue == INT_MIN)
			lowerMaxValue = 0;
		
		return ((static_cast<double>(lowerMaxValue) + static_cast<double>(upperMinValue)) / 2);
	}

	bool Histogram::removeKmeansValues(double minBoundary, double maxBoundary)
	{
		bool success = true;
		int maxVal = INT_MIN;
		for (auto itr = begin(this->extremumList); itr != end(this->extremumList); ++itr)
		{
			if (values[itr->first] > maxVal)
				maxVal = values[itr->first];
			if (itr->second == ExtremumType::TYPE_MAX && 
				values[itr->first] > maxBoundary)
			{
				auto jtr = next(itr);
				for (; jtr != end(this->extremumList) &&
					(jtr->second == ExtremumType::TYPE_MIN || values[jtr->first] < maxBoundary); ++jtr)
				{
					// delete minimum in upper minimum cluster.
					if (jtr->second == ExtremumType::TYPE_MIN && values[jtr->first] > minBoundary)
					{
						jtr = this->extremumList.erase(jtr);
						--jtr;
					}
				}

				if (jtr == end(this->extremumList))
				{
					goto outerLoop;
				}
				else 
				{
					int minValue = INT_MAX;
					decltype(itr) minPtr;
					for (auto ktr = next(itr); ktr != jtr; ++ktr)
					{
						if (ktr->second == ExtremumType::TYPE_MIN && minValue > values[ktr->first])
						{
							minValue = values[ktr->first];
							minPtr = ktr;
						}
					}
					if (minValue != INT_MAX)
					{
						for (auto ktr = next(itr); ktr != jtr; ++ktr)
						{
							if (ktr != minPtr && ktr->second == ExtremumType::TYPE_MIN && values[ktr->first] > 0)
							{
								ktr = this->extremumList.erase(ktr);
								--ktr;
							}
						}
					}
				}
			}
		}

	outerLoop:

		return success;
	}

	void Histogram::detectLines()
	{
		// erase all maximums.
		for (auto itr = std::begin(this->extremumList); itr != std::end(this->extremumList); )
		{
			if (itr->second == ExtremumType::TYPE_MAX)
			{
				itr = this->extremumList.erase(itr);
			}
			else
			{
				++itr;
			}
		}

		// add visible lines to extremumList.
		auto itr = std::begin(this->extremumList);
		for (const auto &line : visibleLines)
		{
			while (itr != std::end(this->extremumList) && itr->first < line.first) ++itr;
			if (itr == std::end(this->extremumList) || itr->first != line.first) {
				this->extremumList.emplace(itr, line.first, line.second);
			}
		}
	}

	/* HistogramManager */

	HistogramManager::HistogramManager(const Common::PngImage& image)
		: HistogramManager(image, image.getWidth(), image.getHeight(), 0, 0)
	{
	}

	HistogramManager::HistogramManager(const Common::PngImage& image,
		unsigned areaWidth, unsigned areaHeight, unsigned offsetWidth, unsigned offsetHeight, bool edgeExist)
		: image(image), areaWidth(areaWidth), areaHeight(areaHeight), 
			offsetWidth(offsetWidth), offsetHeight(offsetHeight), pHistogramX(nullptr), pHistogramY(nullptr),
			edgeExist(edgeExist)
	{
	}

	HistogramManager::HistogramManager(const HistogramManager& h)
		: HistogramManager(h.image, h.areaWidth, h.areaHeight, h.offsetWidth, h.offsetHeight, h.edgeExist)
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
			pHistogramX = std::make_shared<Histogram>(type, image, offsetWidth, offsetHeight, areaWidth, areaHeight, edgeExist);
			if (pHistogramX && !(success = pHistogramX->calculateValues()))
				pHistogramX.reset();
			break;
			
		case HistogramType::TYPE_Y:
			pHistogramY = std::make_shared<Histogram>(type, image, offsetWidth, offsetHeight, areaHeight, areaWidth, edgeExist);
			if (pHistogramY && !(success = pHistogramY->calculateValues()))
				pHistogramY.reset();
			break;
		}

		return success;
	}

	bool HistogramManager::detectVisibleLines(HistogramType type)
	{
		bool success;

		switch (type)
		{
		case HistogramType::TYPE_X:
			success = pHistogramX->detectVisibleLines();
			break;

		case HistogramType::TYPE_Y:
			success = pHistogramY->detectVisibleLines();
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
	std::vector<Common::Line> HistogramManager::getLineInfo()
	{
		// top bottom left right
		std::vector<Common::Line> lineVector;

		pHistogramX->detectLines();
		pHistogramY->detectLines();

		auto xExtremum  = pHistogramX->getExtremumList();
		auto yExtremum  = pHistogramY->getExtremumList();

		// merge adjacent lines.
		// * TODO : select middle things.
		for (auto itr = std::begin(xExtremum); itr != std::prev(std::end(xExtremum)); )
		{
			auto jtr = std::next(itr);
			if (itr->first + 1 >= jtr->first) {
				itr = xExtremum.erase(itr);
			}
			else {
				++itr;
			}
		}
		for (auto itr = std::begin(yExtremum); itr != std::prev(std::end(yExtremum)); )
		{
			auto jtr = std::next(itr);
			if (itr->first + 1 >= jtr->first) {
				itr = yExtremum.erase(itr);
			}
			else {
				++itr;
			}
		}

		if (this->edgeExist)
		{
			if (xExtremum.empty() || xExtremum.front().first > 0)
				xExtremum.emplace_front(0, ExtremumType::TYPE_MIN);
			if (xExtremum.empty() || xExtremum.back().first < this->areaWidth - 1)
				xExtremum.emplace_back(this->areaWidth - 1, ExtremumType::TYPE_MIN);

			if (yExtremum.empty() || yExtremum.front().first > 0)
				yExtremum.emplace_front(0, ExtremumType::TYPE_MIN);
			if (yExtremum.empty() || yExtremum.back().first < this->areaHeight - 1)
				yExtremum.emplace_back(this->areaHeight - 1, ExtremumType::TYPE_MIN);
		}

		for (const auto &x : xExtremum) {
			lineVector.emplace_back(Common::LineType::LINE_VERTICAL, x.first);
		}
		for (const auto &y : yExtremum) {
			lineVector.emplace_back(Common::LineType::LINE_HORIZONTAL, y.first);
		}

		/*
		if (yExtremum.empty() || xExtremum.empty()) 
		{
			return tableVector;
		}

		int top, bottom, left, right;
		for (auto itr = begin(yExtremum); itr != prev(end(yExtremum)); ++itr)
		{
			top = itr->first;
			bottom = next(itr)->first;
			for (auto jtr = begin(xExtremum); jtr != prev(end(xExtremum)); ++jtr)
			{
				left = jtr->first;
				right = next(jtr)->first;
				tableVector.emplace_back(top, bottom, left, right);
			}
		}
		*/
		return lineVector;
	}
}