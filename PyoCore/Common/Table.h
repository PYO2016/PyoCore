#pragma once

#include <string>
#include <vector>

namespace Common
{
	class Cell
	{
	public:
		Cell(int top, int bottom, int left, int right, const std::wstring& innerString);
		~Cell() = default;

		inline int getTop();
		inline int getLeft();
		inline int getRight();
		inline int getBottom();
		inline int getWidth();
		inline int getHeight();
		inline std::wstring getInnerString();

	private:
		int top;
		int left;
		int right;
		int bottom;
		std::wstring innerString;
		/* innerImage ?? */
		/* sparse blocks ?? */
	};

	class Table
	{
	public:
		Table() = default;
		~Table() = default;

	private:
		std::vector<Cell> cells;
	};

	/* inline functions */

	inline int Cell::getTop()
	{
		return top;
	}

	inline int Cell::getLeft()
	{
		return left;
	}

	inline int Cell::getRight()
	{
		return right;
	}

	inline int Cell::getBottom()
	{
		return bottom;
	}

	inline int Cell::getWidth()
	{
		return (left < right ? right - left : left - right);
	}

	inline int Cell::getHeight()
	{
		return (bottom < top ? top - bottom : bottom - top);
	}

	inline std::wstring Cell::getInnerString()
	{
		return innerString;
	}
}

