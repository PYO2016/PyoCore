#include "TableExporter.h"
#include <algorithm>
#include <set>
#include <map>
#include <vector>

namespace TableDetection
{
	std::wstring TableExporter::exportTable(const Common::Table& table)
	{
		std::vector<Common::Cell> cells = table.getCells();
		std::set<int, std::less<int>> topSet, leftSet;
		std::map<int, int> rowMap, colMap;	// top or left -> idx(th)
		int maxBottom = 0, maxRight = 0;
		int rowCnt = 0, colCnt = 0;

		for (const auto &cell : cells) {
			topSet.insert(cell.getTop());
			leftSet.insert(cell.getLeft());
			if (maxBottom < cell.getBottom())
				maxBottom = cell.getBottom();
			if (maxRight < cell.getRight())
				maxRight = cell.getRight();
		}
		for (const auto &top : topSet) {
			rowMap[top] = rowCnt++;
		}
		rowMap[maxBottom] = rowCnt;
		for (const auto &left : leftSet) {
			colMap[left] = colCnt++;
		}
		colMap[maxRight] = colCnt;

		for (const auto &cell : cells) {
			int row = rowMap[cell.getTop()];
			int col = colMap[cell.getLeft()];
			int rowspan = rowMap[cell.getBottom()] - row;
			int colspan = colMap[cell.getRight()] - col;
		}

		/* sort cells by row asc, col asc */
		/* iterate sorted cells and generate HTML and CSS */
		
		/* HTML generating rule 
			<table class='PYO2016-table'>
				~~~
			</table>
		*/
		/* CSS rule
			<table class='PYO2016-table'>

			<tr class='####'>
				#### : row-x (x means row(th))

			<td ... class='#####'>
				##### : cell-x (x is index of sorted cells.)

			Issue) 
				thead, tbody, tfoot
				th ?? (need more informations to apply these tags)
		*/

		/************************************/
		/* sort rule
			*assume that most top and most left point is (0,0).
			1. top asc
			2. left asc
		*/
		/*std::sort(std::begin(cells), std::end(cells),
			[](const auto &c1, const auto &c2) {
			if (c1.getTop() < c2.getTop())
				return true;
			else if (c1.getTop() > c2.getTop())
				return false;
			else {
				if (c1.getLeft() < c2.getLeft())
					return true;
				else
					return false;
			}
		});
		*/
	}
}