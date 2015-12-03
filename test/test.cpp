#include <iostream>
#include <vector>
#include <utility>
#include <functional>
#include <algorithm>

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

void testMedianFilter(void)
{
	/*
	const int length = 10;
	int values[length] = { 1,10,3,40,3,20,1,20,3,40 };
	*/
	const int length = 10;
	int values[length] = { 1,1,30,4,3,200,1,2,3,40 };

	const int range = 5;	// must be odd number.
	const int halfRange = range / 2;
	std::vector<std::pair<int, int> > valVector;	// val, idx

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
		
		printf("[*%d] %d\n", i, valVector.size());
		for (auto &v : valVector)
			printf("%d ", v.first);
		printf("\n");
		
		values[i] = valVector[valVector.size() / 2].first;
	}

	cout << "-------- after --------" << endl;
	for (int i = 0; i < length; ++i) {
		cout << values[i] << " ";
	}
	cout << endl;
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

	return 0;
}