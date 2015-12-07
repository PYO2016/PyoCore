#define PYOCORE_DLL
#include "PyoCore.h"

#include <cstdio>
#include <cstring>

#include "TableDetection/TableDetector.h"
#include "Common/EncodingConverter.h"

namespace PyoCore
{

	/************** Import Test **************/

	void helloWorld(void)
	{
		printf("hello, world! 안녕, 세상아!\n");
		wprintf(L"I'm 태국!\n");
	}

	BOOL importTestW(LPCWSTR imageFileName, ImageFileType imageFileType, LPWSTR outputFileName)
	{
		wprintf(L"importTestW() called!\n");
		wprintf(L"Arg : %s / %d\n", imageFileName, imageFileType);
		wcscpy(outputFileName, L"WIDE Copy 와이드 카피");

		return TRUE;
	}

	BOOL importTestA(LPCSTR imageFileName, ImageFileType imageFileType, LPSTR outputFileName)
	{
		printf("importTestA() called!\n");
		printf("Arg : %s / %d\n", imageFileName, imageFileType);
		strcpy(outputFileName, "ANSI Copy 안시 카피");

		return TRUE;
	}


	/************** Processing image file **************/

	BOOL processImageFileW(LPCWSTR imageFileName, ImageFileType imageFileType, LPCWSTR outputFileName)
	{
		TableDetection::TableDetector tableDectector;

		return tableDectector.process(imageFileName, outputFileName);
	}

	BOOL processImageFileA(LPCSTR imageFileName, ImageFileType imageFileType, LPCSTR outputFileName)
	{
		return processImageFileW(
			Common::EncodingConverter::s2ws(imageFileName).c_str(),
			imageFileType,
			Common::EncodingConverter::s2ws(outputFileName).c_str());
	}


	/************** Handling Error **************/

	int errorCode;

	int getErrorCode(void)
	{
		return errorCode;
	}

}