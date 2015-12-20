#include "Table.h"

namespace Common
{
	/* Cell */

	Cell::Cell(int top, int bottom, int left, int right, const std::wstring& innerString)
	{
		this->top = top;
		this->bottom = bottom;
		this->left = left;
		this->right = right;
		this->innerString = innerString;
	}

	/* Table */

	Table::Table(const std::vector<Cell>& cells)
		: cells(cells)
	{
	}

	Table::Table(std::vector<Cell>&& cells)
		: cells(cells)
	{
	}
}