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

		for (auto &cell : cells) {
			int row = rowMap[cell.getTop()];
			int col = colMap[cell.getLeft()];
			int rowspan = rowMap[cell.getBottom()] - row;
			int colspan = colMap[cell.getRight()] - col;
			cell.setRow(row);
			cell.setCol(col);
			cell.setRowspan(rowspan);
			cell.setColspan(colspan);
		}

		return generateHtml(cells);
	}

	std::wstring TableExporter::generateHtml(std::vector<Common::Cell> &cells)
	{
		/* sort rule
			*assume that most top and most left cell is (row,col) = (0,0).
			1. row asc
			2. col asc
		*/
		std::sort(std::begin(cells), std::end(cells),
			[](const auto &c1, const auto &c2) {
			if (c1.getRow() < c2.getRow())
				return true;
			else if (c1.getRow() > c2.getRow())
				return false;
			else {
				if (c1.getCol() < c2.getCol())
					return true;
				else
					return false;
			}
		});

		/* HTML generating example
			<table class='PYO2016-table'>
				~~~
			</table>
		*/
		/* CSS rule
			<table border=1 class='PYO2016-table'>

			<tr class='####'>
				#### : row-x (x means row(th))

			<td ... class='#####'>
				##### : cell-x (x is index of sorted cells.)

			Issue)
				thead, tbody, tfoot
				th ?? (need more informations to apply these tags)
		*/
		std::wstring html;
		int row = -1;
		int idx = 0;
		html += L"<table class='PYO2016-table'>\n";
		for (const auto &cell : cells) {
			if (row < cell.getRow()) {
				if(row >= 0) html += L"  </tr>\n";
				html += L"  <tr class='row-" + std::to_wstring(cell.getRow()) + L"'>\n";
				row = cell.getRow();
			}
			html += L"    <td rowspan=" + std::to_wstring(cell.getRowspan()) +
				L" colspan=" + std::to_wstring(cell.getColspan()) +
				L" class='cell-" + std::to_wstring(idx++) + L"'>" +
				cell.getInnerString() + L"</td>\n";
		}
		html += L"</table>";

		std::wstring css(
			L"<style type='text/css'>\n"
			L"  .PYO2016-table { \n"
			L"    border-collapse : collapse; \n"
			L"    text-align : center; \n"
			L"  } \n"
			L"  .PYO2016-table table, .PYO2016-table th, .PYO2016-table td { \n"
			L"    border : 1px gray solid; \n"
			L"  } \n"
			);

		idx = 0;
		for (const auto &cell : cells) {
			css += L"  .cell-" + std::to_wstring(idx++) + L" { \n" +
				L"    width : " + std::to_wstring(cell.getWidth()) + L"px; \n" +
				L"    height : " + std::to_wstring(cell.getHeight()) + L"px; \n" +
				L"  } \n";
		}

		css += L"</style>";

		return css + L"\n\n" + html;
	}
}

/* sort cells by row asc, col asc */
/* iterate sorted cells and generate HTML and CSS */

