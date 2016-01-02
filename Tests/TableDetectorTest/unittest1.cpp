#include "stdafx.h"
#include "CppUnitTest.h"
#include <fstream>
#include <cstdlib>

#include "../../PyoCore/Common/EncodingConverter.h"
#define private public
#include "../../PyoCore/TableDetection/TableDetector.h"
#include "../../PyoCore/TableDetection/TableExporter.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TableDetectorTest
{		
	TEST_CLASS(TableDetectorTest)
	{
	public:
		
		TEST_METHOD(TestProcess)
		{
			// TODO: 테스트 코드를 여기에 입력합니다.
			_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
			_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
			TableDetection::TableDetector td;
			std::wstring configFile = L"CONFIG.txt";
			std::wifstream cif(configFile);

			std::wstring prefix;
			cif >> prefix;
			cif.close();

			std::wstring imageFile = prefix + L".png";
			std::wstring outputFile = prefix + L".html";
			Logger::WriteMessage(td.process(imageFile, L"junk", true) ? "true" : "false");
			Logger::WriteMessage(td.result.c_str());
			std::wofstream of(outputFile);
			of << td.result.c_str();
			of.close();
			system(("explorer.exe " + Common::EncodingConverter::ws2s(outputFile)).c_str());
		}

	};
}