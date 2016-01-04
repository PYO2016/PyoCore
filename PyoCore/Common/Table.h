#pragma once

#include <string>
#include <vector>
#include <tuple>
#include "EncodingConverter.h"

namespace Common
{
	enum class LineType : int
	{
		LINE_HORIZONTAL = 0,
		LINE_VERTICAL
	};

	class Line
	{
	public:
		Line(LineType type, int offset);
		~Line() = default;

		inline LineType getType() const;
		inline int getOffset() const;

	private:
		LineType type;
		int offset;
	};

	class Cell
	{
	public:
		Cell(int top, int bottom, int left, int right, const std::wstring& innerString = L"");
		~Cell() = default;

		inline int getTop() const;
		inline int getLeft() const;
		inline int getRight() const;
		inline int getBottom() const;
		inline int getWidth() const;
		inline int getHeight() const;
		inline std::wstring getInnerString() const;

		inline int getRow() const;
		inline int getCol() const;
		inline int getRowspan() const;
		inline int getColspan() const;
		inline void setRow(int row);
		inline void setCol(int col);
		inline void setRowspan(int rowspan);
		inline void setColspan(int colspan);
		inline void setInnerString(const char*);
		inline void setInnerString(std::wstring);

	private:
		int top;
		int left;
		int right;
		int bottom;
		std::wstring innerString;
		/* innerImage ?? */
		/* sparse blocks ?? */
		
		int row, col;
		int rowspan, colspan;
	};

	class Table
	{
	public:
		Table() = default;
		Table(const std::vector<Cell>& cells);
		Table(std::vector<Cell>&& cells);
		~Table() = default;

		inline void addCell(int top, int bottom, int left, int right);
		inline void addCell(std::tuple<int,int,int,int> cell);
		inline std::vector<Cell>& getCells();
		inline const std::vector<Cell>& getCells() const;
	private:
		std::vector<Cell> cells;

		/*
		int rowCnt, colCnt;
		*/
	};

	/* inline functions */

	/* Line */

	inline LineType Line::getType() const
	{
		return type;
	}
	inline int Line::getOffset() const
	{
		return offset;
	}

	/* Cell */

	inline int Cell::getRow() const
	{
		return row;
	}
	inline int Cell::getCol() const 
	{
		return col;
	}
	inline int Cell::getRowspan() const 
	{
		return rowspan;
	}
	inline int Cell::getColspan() const 
	{
		return colspan;
	}
	inline void Cell::setRow(int row)
	{
		this->row = row;
	}
	inline void Cell::setCol(int col)
	{
		this->col = col;
	}
	inline void Cell::setRowspan(int rowspan)
	{
		this->rowspan = rowspan;
	}
	inline void Cell::setColspan(int colspan)
	{
		this->colspan = colspan;
	}
	inline void Cell::setInnerString(const char *text)
	{
		this->innerString = EncodingConverter::s2ws(text);
	}
	inline void Cell::setInnerString(std::wstring text)
	{
		this->innerString = text;
	}
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

	/* Table */
	inline void Table::addCell(int top, int bottom, int left, int right)
	{
		cells.emplace_back(top, bottom, left, right);
	}
	inline void Table::addCell(std::tuple<int, int, int, int> cell)
	{
		int top = std::get<0>(cell);
		int bottom = std::get<1>(cell);
		int left = std::get<2>(cell);
		int right = std::get<3>(cell);
		addCell(top, bottom, left, right);
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

