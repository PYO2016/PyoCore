#include "stdafx.h"
#include "CppUnitTest.h"

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
			// TODO: �׽�Ʈ �ڵ带 ���⿡ �Է��մϴ�.
			_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
			_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
			TableDetection::TableDetector td;
			Logger::WriteMessage(td.process(L"test.png", L"junk", true) ? "true" : "false");
			Logger::WriteMessage(td.result.c_str());
		}

	};
}