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
		SparseBlock(box b);
		virtual ~SparseBlock() = default;

		inline int getTop();
		inline int getLeft();
		inline int getRight();
		inline int getBottom();
		inline int getWidth();
		inline int getHeight();
		inline std::wstring getText();
		inline void setText(const char *text);
		inline void setText(std::wstring text);
		inline int getRealArea();
		inline bool setRealArea(int value);
		inline bool operator==(SparseBlock r);
		inline bool operator!=(SparseBlock r);

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
		return getRight() - getLeft() + 1;
	}
	inline int SparseBlock::getHeight()
	{
		return getBottom() - getTop() + 1;
	}
	inline std::wstring SparseBlock::getText()
	{
		return text;
	}
	inline void SparseBlock::setText(const char *text)
	{
		this->text = EncodingConverter::s2ws(text);
	}
	inline void SparseBlock::setText(std::wstring text)
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
	inline bool SparseBlock::operator==(SparseBlock r)
	{
		return r.getBottom() == this->getBottom() && r.getLeft() == this->getLeft()
			&& r.getTop() == this->getTop() && r.getRight() == this->getRight();
	}
	inline bool SparseBlock::operator!=(SparseBlock r)
	{
		return !(*this == r);
	}
}