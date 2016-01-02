#pragma once
#include <string>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>

using namespace std;

namespace Common 
{
	typedef boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian> point;
	typedef boost::geometry::model::box<point> box;
	class SparseBlock : public box
	{
	public:
		SparseBlock(int top, int bottom, int left, int right, const char* text);
		~SparseBlock();
		inline int getTop()
		{
			return min_corner().get<1>();
		}

		inline int getLeft()
		{
			return min_corner().get<0>();
		}

		inline int getRight()
		{
			return max_corner().get<0>();
		}

		inline int getBottom()
		{
			return max_corner().get<1>();
		}

		inline int getWidth()
		{
			return (getLeft() < getRight() ? getRight() - getLeft() : getLeft() - getRight());
		}

		inline int getHeight()
		{
			return (getBottom() < getTop() ? getTop() - getBottom() : getBottom() - getTop());
		}

		inline wstring getText()
		{
			return text;
		}

	private:
		wstring text;
	};
}