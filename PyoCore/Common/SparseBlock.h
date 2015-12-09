#pragma once
#include <string>

using namespace std;

namespace PyoCore {

	class SparseBlock
	{
	public:
		SparseBlock(int top, int bottom, int left, int right, const char* text);
		~SparseBlock();
		inline int getTop()
		{
			return top;
		}

		inline int getLeft()
		{
			return left;
		}

		inline int getRight()
		{
			return right;
		}

		inline int getBottom()
		{
			return bottom;
		}

		inline int getWidth()
		{
			return (left < right ? right - left : left - right);
		}

		inline int getHeight()
		{
			return (bottom < top ? top - bottom : bottom - top);
		}

		inline string getText()
		{
			return text;
		}

	private:
		int top;
		int left;
		int right;
		int bottom;
		string text;
	};
}