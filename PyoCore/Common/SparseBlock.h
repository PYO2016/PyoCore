#pragma once
#include <string>
#include "EncodingConverter.h"
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>

namespace Common 
{
	typedef boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian> point;
	typedef boost::geometry::model::box<point> box;
	class SparseBlock : public box
	{
	public:
		SparseBlock(point p1, point p2, int realArea = 0, std::wstring text = L"");
		~SparseBlock();

		inline int getTop();
		inline int getLeft();
		inline int getRight();
		inline int getBottom();
		inline int getWidth();
		inline int getHeight();
		inline std::wstring getText();
		inline std::wstring SparseBlock::setText(const char *text);
		inline std::wstring setText(std::wstring text);
		inline int getRealArea();
		inline bool setRealArea(int value);

	private:
		std::wstring text;
		int realArea;
	};

	/* inline functions */

	inline int SparseBlock::getTop()
	{
		return min_corner().get<1>();
	}
	inline int SparseBlock::getLeft()
	{
		return min_corner().get<0>();
	}
	inline int SparseBlock::getRight()
	{
		return max_corner().get<0>();
	}
	inline int SparseBlock::getBottom()
	{
		return max_corner().get<1>();
	}
	inline int SparseBlock::getWidth()
	{
		return (getLeft() < getRight() ? getRight() - getLeft() : getLeft() - getRight());
	}
	inline int SparseBlock::getHeight()
	{
		return (getBottom() < getTop() ? getTop() - getBottom() : getBottom() - getTop());
	}
	inline std::wstring SparseBlock::getText()
	{
		return text;
	}
	inline std::wstring SparseBlock::setText(const char *text)
	{
		this->text = EncodingConverter::s2ws(text);
	}
	inline std::wstring SparseBlock::setText(std::wstring text)
	{
		this->text = text;
	}
	inline int SparseBlock::getRealArea()
	{
		return this->realArea;
	}
	inline bool SparseBlock::setRealArea(int value)
	{
		this->realArea = value;
		return true;
	}
}