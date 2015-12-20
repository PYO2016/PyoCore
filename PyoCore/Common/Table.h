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

		inline int getTop() const;
		inline int getLeft() const;
		inline int getRight() const;
		inline int getBottom() const;
		inline int getWidth() const;
		inline int getHeight() const;
		inline std::wstring getInnerString() const;

	private:
		int top;
		int left;
		int right;
		int bottom;
		std::wstring innerString;
		/* innerImage ?? */
		/* sparse blocks ?? */
		
		/*
		int row, col;
		int rowspan, colspan;
		*/
	};

	class Table
	{
	public:
		Table() = default;
		~Table() = default;

		inline std::vector<Cell>& getCells();
		inline const std::vector<Cell>& getCells() const;
	private:
		std::vector<Cell> cells;

		/*
		int rowCnt, colCnt;
		*/
	};

	/* inline functions */

	inline int Cell::getTop() const
	{
		return top;
	}

	inline int Cell::getLeft() const
	{
		return left;
	}

	inline int Cell::getRight() const
	{
		return right;
	}

	inline int Cell::getBottom() const
	{
		return bottom;
	}

	inline int Cell::getWidth() const
	{
		return (left < right ? right - left : left - right);
	}

	inline int Cell::getHeight() const
	{
		return (bottom < top ? top - bottom : bottom - top);
	}

	inline std::wstring Cell::getInnerString() const
	{
		return innerString;
	}

	inline std::vector<Cell>& Table::getCells()
	{
		return cells;
	}
	inline const std::vector<Cell>& Table::getCells() const
	{
		return cells;
	}
}

