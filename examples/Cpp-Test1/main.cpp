#include "../../PyoCore/PyoCore.h"

#include <iostream>

using namespace std;

int main(void)
{
	cout << "*********** C++ DLL Test 1 ***********" << endl << endl;

	PyoCore::processImageFile(L"inputFileName.png", PyoCore::IMAGE_FILE_TYPE_PNG, L"outputFileName.pyo", true);

	return 0;
}