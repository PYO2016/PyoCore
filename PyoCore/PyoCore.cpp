#define PYOCORE_DLL
#include "PyoCore.h"
#include "PyoCoreInternal.h"

#include <cstdio>
#include <cstring>

#include "TableDetection/TableDetector.h"
#include "Common/EncodingConverter.h"

namespace PyoCore
{
	std::mutex tesseract_mtx;

	/************** Handling Error **************/

	int errorCode;

	int getErrorCode(void)
	{
		return errorCode;
	}

	/************** Processing image file **************/

	int processImageFileW(LPCWSTR imageFileName, ImageFileType imageFileType,
		LPWSTR resultBuffer, UINT32 resultBufferLen, BOOL isDebug)
	{
		bool success = false;
		TableDetection::TableDetector tableDectector;
		std::wstring resultString;

		try {
			if (resultBufferLen == 0) {
				errorCode = ERROR_UNKNOWN;
				//goto END;
				return -1;
			}

			if (imageFileType != IMAGE_FILE_TYPE_PNG) {
				errorCode = ERROR_IMAGE_FILE_TYPE;
				//goto END;
				return -2;
			}

			if (!tableDectector.process(imageFileName, resultString, static_cast<bool>(isDebug))) {
				errorCode = ERROR_UNKNOWN;
				//goto END;
				wcscpy_s(resultBuffer, resultBufferLen, resultString.c_str());
				return -3;
			}

			if (resultString.length() + 1 >= resultBufferLen) {
				errorCode = ERROR_UNKNOWN;
				//goto END;
				return -4;
			}

			wcscpy_s(resultBuffer, resultBufferLen, resultString.c_str());
		}
		catch (std::exception e) {
			// when unhandled exception.
			//return FALSE;
			wcscpy_s(resultBuffer, resultBufferLen, Common::EncodingConverter::s2ws(e.what()).c_str());
			return -5;
		}

		return 1;
		//success = true;
	//END:

		//return static_cast<BOOL>(success);
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