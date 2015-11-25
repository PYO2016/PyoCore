#define PYOCORE_DLL
#include "PyoCore.h"

#include <cstdio>
#include <cstring>

/************** Import Test **************/

void PyoCore::helloWorld(void)
{
	printf("hello, world! 안녕, 세상아!\n");
	wprintf(L"I'm 태국!\n");
}


/************** Processing image file **************/

BOOL PyoCore::processImageFileW(LPCWSTR imageFileName, ImageFileType imageFileType, LPWSTR outputFileName)
{
	/* Temporary Code for test. */
	wprintf(L"processImageFileW() called!\n");
	wprintf(L"Arg : %s / %d\n", imageFileName, imageFileType);
	wcscpy(outputFileName, L"WIDE Copy 와이드 카피");

	return TRUE;
}

BOOL PyoCore::processImageFileA(LPCSTR imageFileName, ImageFileType imageFileType, LPSTR outputFileName)
{
	/* Temporary Code for test. */
	printf("processImageFileA() called!\n");
	printf("Arg : %s / %d\n", imageFileName, imageFileType);
	strcpy(outputFileName, "ANSI Copy 안시 카피");

	return TRUE;

	/* internally call TableDetection::processImageFileW after MultiByteToWideChar. */
}


/************** Handling Error **************/

int errorCode;

int PyoCore::getErrorCode(void)
{
	return errorCode;
}