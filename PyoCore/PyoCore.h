#pragma once

#ifdef PYOCORE_DLL

#define PYOCORE_API extern "C" __declspec(dllexport)

#else

#define PYOCORE_API extern "C" __declspec(dllimport)

#endif

#include <windows.h>

/* Interface for Algorithm DLL */
namespace PyoCore
{
	/************** Processing image file **************/

	typedef int ImageFileType;

	/* Available image file types */
	enum {
		IMAGE_FILE_TYPE_PNG,

		/* Not image file type. Just for the number of image file types */
		IMAGE_FILE_TYPE_CNT
	};

	/*
	 * Parameters *
	 *		imageFileName : the name of image file.
	 *		imageFileType : the type of image file.
	 *		resultBuffer : the result buffer where the result string of processing is stored.
	 *		resultBufferLen : the result buffer length. (include null character)
	 * Return Value *
	 *		TRUE if success, FALSE otherwise.
	 *		Use getErrorCode() if you want to know the error code.
	 * Description *
	 */
#ifdef UNICODE
#define processImageFile processImageFileW
#else
#define processImageFile processImageFileA
#endif
	PYOCORE_API BOOL processImageFileW(LPCWSTR imageFileName, ImageFileType imageFileType, 
		LPWSTR resultBuffer, UINT32 resultBufferLen, BOOL isDebug);
	PYOCORE_API BOOL processImageFileA(LPCSTR imageFileName, ImageFileType imageFileType, 
		LPSTR resultBuffer, UINT32 resultBufferLen, BOOL isDebug);


	/************** Handling Error **************/

	/* Available error codes */
	enum {
		/* No error */
		ERROR_NONE,
		/* Unknown error */
		ERROR_UNKNOWN,
		/* Not supported image file type */
		ERROR_IMAGE_FILE_TYPE,

		/* Not error code. Just for the number of error codes */
		ERROR_CODE_CNT
	};

	/* A function for getting error code */
	PYOCORE_API int getErrorCode(void);
}