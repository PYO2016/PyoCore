#include "../../PyoCore/PyoCore.h"

#include <cstdio>

int main(void)
{
	printf("C++ import DLL examples\n\n");

	PyoCore::helloWorld();
	printf("\n");

	CHAR str1[10000];
	PyoCore::importTestA("ANSI-test", PyoCore::IMAGE_FILE_TYPE_PNG, str1);
	printf("%s\n\n", str1);

	WCHAR str2[10000];
	PyoCore::importTestW(L"WIDE-test", PyoCore::IMAGE_FILE_TYPE_PNG, str2);
	wprintf(L"%s\n\n", str2);

	int errorCode = PyoCore::getErrorCode();
	printf("ErrorCode : %d\n", errorCode);

	return 0;
}