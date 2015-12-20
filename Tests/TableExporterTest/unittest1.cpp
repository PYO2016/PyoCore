#include "stdafx.h"
#include "CppUnitTest.h"
#include "../../PyoCore/TableDetection/TableExporter.h"
#include "../../PyoCore/Common/Table.h"
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TableDetectionTest
{		
	TEST_CLASS(TableExporterTest)
	{
	public:
		
		TEST_METHOD(TestExportTable)
		{
			// TODO: 테스트 코드를 여기에 입력합니다.
			std::vector<Common::Cell> cells =
			{
				Common::Cell(0,200,0,200,L"cell-a"),
				Common::Cell(500,600,300,400,L"cell-b"),
				Common::Cell(200,400,100,200,L"cell-c"),
				Common::Cell(200,300,0,100,L"cell-d"),
				Common::Cell(300,400,200,400,L"cell-e"),
				Common::Cell(0,300,200,400,L"cell-f"),
				Common::Cell(300,400,0,100,L"cell-g"),
				Common::Cell(400,500,0,400,L"cell-h"),
				Common::Cell(500,600,0,300,L"cell-i")
			};
			std::wstring html = TableDetection::TableExporter::exportTable(cells);
			Logger::WriteMessage(html.c_str());
		}

	};
}