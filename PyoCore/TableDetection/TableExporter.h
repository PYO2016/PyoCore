#pragma once

namespace TableDetection
{
	class TableExporter
	{
	public:
		TableExporter() = default;
		virtual ~TableExporter() = default;
		TableExporter(const TableExporter &) = delete;
		TableExporter& operator=(const TableExporter &) = delete;
		TableExporter(TableExporter &&) = delete;
		TableExporter& operator=(TableExporter &&) = delete;
		

	private:
	};
}

/*
Attributes List...
...etc...

Method List...
exportTable
*/