#pragma once

#include <string>
#include "../Common/Table.h"

namespace TableDetection
{
	class TableExporter
	{
	public:
		TableExporter() = delete;
		/*
		virtual ~TableExporter() = default;
		TableExporter(const TableExporter &) = delete;
		TableExporter& operator=(const TableExporter &) = delete;
		TableExporter(TableExporter &&) = delete;
		TableExporter& operator=(TableExporter &&) = delete;
		*/

		static std::wstring ExportTable(const Common::Table& table);

	private:
		static std::wstring GenerateHtml(std::vector<Common::Cell> &cells);
	};
}