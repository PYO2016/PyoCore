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

	/************** Handling Error **************/

	int errorCode;

	int getErrorCode(void)
	{
		return errorCode;
	}

	/************** Processing image file **************/

	BOOL processImageFileW(LPCWSTR imageFileName, ImageFileType imageFileType,
		LPWSTR resultBuffer, UINT32 resultBufferLen, BOOL isDebug)
	{
		bool success = false;
		TableDetection::TableDetector tableDectector;
		std::wstring resultString;

		if (resultBufferLen == 0) {
			errorCode = ERROR_UNKNOWN;
			goto END;
		}

		if (imageFileType != IMAGE_FILE_TYPE_PNG) {
			errorCode = ERROR_IMAGE_FILE_TYPE;
			goto END;
		}

		if (!tableDectector.process(imageFileName, resultString, static_cast<bool>(isDebug))) {
			errorCode = ERROR_UNKNOWN;
			goto END;
		}

		if (resultString.length() + 1 >= resultBufferLen) {
			errorCode = ERROR_UNKNOWN;
			goto END;
		}

		wcscpy_s(resultBuffer, resultBufferLen, resultString.c_str());

		success = true;
	END:

		return static_cast<BOOL>(success);
	}

	BOOL processImageFileA(LPCSTR imageFileName, ImageFileType imageFileType,
		LPSTR resultBuffer, UINT32 resultBufferLen, BOOL isDebug)
	{
		bool success = false;
		LPWSTR tempString = NULL;
		
		if (resultBufferLen == 0) {
			errorCode = ERROR_UNKNOWN;
			goto END;
		}
		tempString = new WCHAR[resultBufferLen];

		if (!processImageFileW(
			Common::EncodingConverter::s2ws(imageFileName).c_str(),
			imageFileType,
			tempString,
			resultBufferLen,
			isDebug))
			goto END;

		{
			std::string s = Common::EncodingConverter::ws2s(tempString);
			if (s.length() + 1 >= resultBufferLen) {
				errorCode = ERROR_UNKNOWN;
				goto END;
			}
			strcpy_s(resultBuffer, resultBufferLen, s.c_str());
		}
		success = true;
	END:;

		if (tempString)
			delete[] tempString;
		return success;
	}
}