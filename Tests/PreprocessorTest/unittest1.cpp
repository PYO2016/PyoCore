#include "stdafx.h"
#include "CppUnitTest.h"
#include "../../PyoCore/Common/PngImage.h"
#include "../../PyoCore/Preprocessing/Preprocessor.h"
#include "../../PyoCore/Common/SparseBlockManager.h"
#include <vector>
#include <string>

// to store queries results
#include <queue>

// just for output
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Common;
using namespace Preprocessing;

namespace PreprocessorTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestSparseBlocksPro)
		{
			std::shared_ptr<PngImage> img = PngImage::LoadPngImage(L"..\\..\\testcases\\test8.png");
			Preprocessor::process(*img);
			img->storeToFile(L"..\\..\\testcases\\t_preprocessed.png");

			SparseBlockManager manager(*img);
			manager.process();

			img->storeToFile(L"..\\..\\testcases\\t_sparsed.png");
		}

		TEST_METHOD(TestSparseBlocks)
		{
			// TODO: 테스트 코드를 여기에 입력합니다.
			std::shared_ptr<PngImage> img = PngImage::LoadPngImage(L"..\\..\\testcases\\test4_1.png");
			img->storeToFile(L"..\\..\\testcases\\t4_copy.png");
			Preprocessor::process(*img);
			img->storeToFile(L"..\\..\\testcases\\t4_gray.png");

			SparseBlockManager manager(*img);
			manager.makeSparseBlock();
			manager.mergeSparseBlock();
			auto s = manager.getSparseBlocks();

			for(auto q : s)
			{
				auto a = q.min_corner().get<0>();
				auto b = q.min_corner().get<1>();
				auto c = q.max_corner().get<0>();
				auto d = q.max_corner().get<1>();
				Logger::WriteMessage(std::to_string(a).c_str());
				Logger::WriteMessage("\n");
				Logger::WriteMessage(std::to_string(b).c_str());
				Logger::WriteMessage("\n");
				Logger::WriteMessage(std::to_string(c).c_str());
				Logger::WriteMessage("\n");
				Logger::WriteMessage(std::to_string(d).c_str());
				Logger::WriteMessage("\n");
				Logger::WriteMessage("\n");
				for (int i = b; i < d; ++i)
				{
					for (int j = a; j < c; j++)
					{
						(*img)[i][j].R = 0;
					}
				}
			}
			img->storeToFile(L"C:/Users/KGWANGMIN/Pictures/4_sparse.png");
		}
	};
}