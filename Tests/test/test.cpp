#include <iostream>
#include <vector>
#include <list>
#include <utility>
#include <functional>
#include <algorithm>
#include <array>

using namespace std;

#pragma pack(push, 1)
struct Pixel
{
	unsigned char R, G, B, A;
};
#pragma pack(pop)

class PixelArray
{
public:
	Pixel& operator[] (int idx)
	{
		return reinterpret_cast<Pixel*>(this)[idx];
	}
};
enum class ExtremumType : int
{
	TYPE_MIN = 0,
	TYPE_MAX
};
enum class KmeansType : int
{
	TYPE_LOWER = 0,
	TYPE_UPPER
};
const int length = 20;
int values[length] = { 0,100,0,20,0,30,0,40,0,50,0,60,0,234,2,3,4,51,2,3 };

void testMedianFilter(void)
{
	/*
	const int length = 10;
	int values[length] = { 1,10,3,40,3,20,1,20,3,40 };
	*/

	const int range = 5;	// must be odd number.
	const int halfRange = range / 2;
	std::array<std::pair<int, int>, range> valArray;	// val, idx
	int cnt = 0;

	cout << "****** Median Filter Test ******" << endl;

	if (length < halfRange || range < 3) {
		// nothing to do for median filter.
		cout << "Case 1" << endl;
		return;
	}

	cout << "Case 2" << endl;
	cout << "-------- before --------" << endl;
	for (int i = 0; i < length; ++i) {
		cout << values[i] << " ";
	}
	cout << endl;

	for (int i = 0; i < halfRange; ++i) {
		//valVector.emplace_back(values[i], i);
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
			//valVector.erase(iter);
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
			//*iter = std::make_pair(values[i + halfRange], i + halfRange);
			(*iter).first = values[i + halfRange];
			(*iter).second = i + halfRange;
		}
		// In case of left corner of histogram,
		else {
			//valVector.emplace_back(values[i + halfRange], i + halfRange);
			valArray[cnt].first = values[i + halfRange];
			valArray[cnt++].second = i + halfRange;
		}

		printf("[*%d] %d\n", i, cnt);
		for (auto iter = std::begin(valArray); iter < std::begin(valArray) + cnt; ++iter)
			printf("%d ", (*iter).first);
		printf("\n");

		std::sort(std::begin(valArray), std::begin(valArray) + cnt);
		values[i] = valArray[cnt / 2].first;
	}

	cout << "-------- after --------" << endl;
	for (int i = 0; i < length; ++i) {
		cout << values[i] << ", ";
	}
	cout << endl;
}

std::list<std::pair<int, ExtremumType>> extremumList;

void filterExtremum()
{
	// find minmax value
	cout << "Filter Test..." << endl;
	ExtremumType currentState, nextState;

	auto extremumListMaxV = values[0];
	if (length < 2) 
	{
		//I think it can not process
		return;
	}
	currentState = ((values[0] - values[1]) > 0) ? ExtremumType::TYPE_MAX: ExtremumType::TYPE_MIN;
	extremumList.emplace_back(0, currentState);
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
			if (extremumListMaxV < values[i - 1])
			{
				extremumListMaxV = values[i - 1];
			}
			extremumList.emplace_back(i - 1, nextState);
			currentState = nextState;
		}
	}
	if (values[length - 1] != values[length - 2])
	{
		extremumList.emplace_back(length - 1, ((values[length - 1] - values[length - 2]) > 0) ? ExtremumType::TYPE_MAX : ExtremumType::TYPE_MIN);
	}

	// remove non-reasonable value
	extremumListMaxV = static_cast<int>(ceil(static_cast<double>(extremumListMaxV) * 0.2));
	for (auto currItr = begin(extremumList); currItr != end(extremumList); ++currItr)
	{
		auto nextItr = next(currItr, 1);
		if (nextItr == end(extremumList))
			continue;
		if (abs(values[nextItr->first] - values[currItr->first]) < extremumListMaxV)
		{
			extremumList.erase(nextItr);
		}
	}
	// data align
	for (auto currItr = begin(extremumList); currItr != end(extremumList); ++currItr)
	{
		auto nextItr = next(currItr, 1);
		if (nextItr == end(extremumList))
			continue;
		while (nextItr != end(extremumList) && currItr->second == nextItr->second)
		{
			if (currItr->second == ExtremumType::TYPE_MAX)
				if (values[currItr->first] < values[nextItr->first])
					currItr->first = nextItr->first;
			else
				if (values[nextItr->first] < values[currItr->first])
					currItr->first = nextItr->first;
			extremumList.erase(nextItr);
			nextItr = next(currItr, 1);
		}
	}
	for (pair<int, ExtremumType> q : extremumList)
	{
		cout << values[q.first];
		if (q.second == ExtremumType::TYPE_MAX)
			cout << "TYPE_MAX" << " ";
		else
			cout << "TYPE_MIN" << " ";
	}
	cout << endl;
	return;
}

double getKmeansBoundary(ExtremumType type)
{
	std::vector<int> forCluster;
	for (std::pair<int, ExtremumType> p : extremumList)
	{
		if (p.second == type)
			forCluster.emplace_back(p.first);
	}
	std::vector<KmeansType> clustered(forCluster.size());
	// for get 1/4th value, 3/4th value
	std::vector<int> forClusterTemp{ forCluster };

	std::vector<int> vForSort(values, values + length);

	std::sort(begin(forClusterTemp), end(forClusterTemp), 
		[&vForSort](int a, int b)
	{
		return vForSort[a] < vForSort[b];
	});

	double lower, upper;

	lower = static_cast<double>(values[forClusterTemp[(forClusterTemp.size() - 1) / 4]]);
	upper = static_cast<double>(values[forClusterTemp[((forClusterTemp.size() - 1) / 4) * 3]]);

	// actually this while loop must divide as 2 group(xCluster, yCluser) but... just my tiresome
	while (true)
	{
		double currentLow = 0, currentUpper = 0;
		for (int i = 0; i < forCluster.size(); i++)
		{
			clustered[i] = (abs(lower - values[forCluster[i]]) > abs(upper - values[forCluster[i]])) ? KmeansType::TYPE_UPPER : KmeansType::TYPE_LOWER;
			// if clustered as lower
			if (clustered[i] == KmeansType::TYPE_LOWER)
			{
				currentLow += values[forCluster[i]];
			}
			else
			{
				currentUpper += values[forCluster[i]];
			}
		}
		currentLow /= forCluster.size();
		currentUpper /= forCluster.size();
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
		if (clustered[i] == KmeansType::TYPE_LOWER && values[forCluster[i]] > lowerMaxValue)
			lowerMaxValue = values[forCluster[i]];
		else if (clustered[i] == KmeansType::TYPE_UPPER && values[forCluster[i]] < upperMinValue)
			upperMinValue = values[forCluster[i]];
	}
	
	return ((static_cast<double>(lowerMaxValue) + static_cast<double>(upperMinValue)) / 2);
}
bool removeKmeansValues(double minBoundary, double maxBoundary)
{
	bool success = true;
	for (auto itr = begin(extremumList); itr != end(extremumList); ++itr)
	{
		if (itr->second == ExtremumType::TYPE_MAX &&
			itr->first > maxBoundary)
		{
			auto jtr = next(itr);
			for (; jtr != end(extremumList) && jtr->second != ExtremumType::TYPE_MAX && jtr->first < maxBoundary; ++jtr);

			if (jtr == end(extremumList))
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
						ktr = extremumList.erase(ktr);
						--ktr;
					}
				}
			}
		}
	}
	outerLoop:
	return success;
}

int main()
{
	/*
	unsigned char arr[12] = { '0','1','2','3','4','5','6','7','8','9','A','B' };

	PixelArray& pixels = *reinterpret_cast<PixelArray*>(&arr[0]);

	cout << pixels[0].R << " " << pixels[0].G << " " << pixels[0].A<< " " << endl
		<< pixels[1].R << " " << pixels[1].G << " " << pixels[1].A << " " << endl
		<< pixels[2].R << " " << pixels[2].G << " " << pixels[2].A << " " << endl;
	*/
	/*
	vector<int> v;

	cout << v.size() << endl;
	*/

	testMedianFilter();

	filterExtremum();
	removeKmeansValues(getKmeansBoundary(ExtremumType::TYPE_MIN), getKmeansBoundary(ExtremumType::TYPE_MAX));


	return 0;
}