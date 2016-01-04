#include "stdafx.h"
#include "CppUnitTest.h"
#include <fstream>
#include <cstdlib>
#include <windows.h>

#include "../../PyoCore/Common/EncodingConverter.h"
#include "../../PyoCore/Common/PngImage.h"
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
			std::wstring dir = L"..\\..\\testcases\\";

			std::wstring configFile = dir + L"CONFIG.txt";
			std::wifstream cif(configFile);

			while (!cif.eof())
			{
				std::wstring prefix;
				cif >> prefix;

				// if comment
				if (prefix[0] == L'#')
					continue;

				Logger::WriteMessage((L"---- Test for \"" + prefix + L"\" ----\n").c_str());

				TableDetection::TableDetector td;
				std::wstring resultString;

				std::wstring imageFile = dir + prefix + L".png";
				std::wstring outputFile = dir + prefix + L".html";

				bool success = td.process(imageFile, resultString, true);
				if (!success)
					Logger::WriteMessage("processing fail!\n");
				else
					Logger::WriteMessage("processing success!\n");

				Assert::IsTrue(success);
				
				//Logger::WriteMessage(resultString.c_str());
				std::wofstream of(outputFile);
				of << resultString.c_str();
				of.close();
				//system(("explorer.exe " + Common::EncodingConverter::ws2s(outputFile)).c_str());

				Common::PngImage result(*td.pImage);
				for (auto &cell : td.table.getCells()) {
					int top = cell.getTop();
					int bottom = cell.getBottom();
					int left = cell.getLeft();
					int right = cell.getRight();
					for (int i = top; i <= bottom; ++i) {
						result[i][left].R = result[i][right].R = 0x0;
						result[i][left].G = result[i][right].G = 0x0;
						result[i][left].B = result[i][right].B = 0x99;
					}
					for (int i = left; i <= right; ++i) {
						result[top][i].R = result[bottom][i].R = 0x0;
						result[top][i].G = result[bottom][i].G = 0x0;
						result[top][i].B = result[bottom][i].B = 0x99;
					}
				}
				std::wstring resultFile = dir + prefix + L".png_result.png";
				result.storeToFile(resultFile);

				system(Common::EncodingConverter::ws2s(L"start mspaint.exe " + resultFile).c_str());
				Sleep(1000);
			}
			cif.close();
		}
	};
}