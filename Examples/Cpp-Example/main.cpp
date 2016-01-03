#include "../../PyoCore/PyoCore.h"

#include <iostream>

using namespace std;

int main(void)
{
	const int len = 10000;
	{
		cout << "*********** PyoCore in C++ Example 1 ***********" << endl << endl;
		wchar_t result[len];
		bool success =
			PyoCore::processImageFileW(L"example-.png", PyoCore::IMAGE_FILE_TYPE_PNG, result, len, true);

		if (success) {
			wcout << result << endl;
		}
		else {
			cout << "fail!" << endl;
		}
	}
	cout << endl << endl;
	{
		cout << "*********** PyoCore in C++ Example 2 ***********" << endl << endl;
		char result[len];
		bool success =
			PyoCore::processImageFileA("example.png", PyoCore::IMAGE_FILE_TYPE_PNG, result, len, true);

		if (success) {
			wcout << result << endl;
		}
		else {
			cout << "fail!" << endl;
		}
	}

	return 0;
}