#pragma once

#include <vector>
#include "../Common/PngImage.h"

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>

#include "opencv2/opencv.hpp"

using namespace std;
using namespace Common;
using namespace cv;

namespace Preprocessing
{
	namespace bg = boost::geometry;
	namespace bgi = boost::geometry::index;

	typedef bg::model::point<int, 2, bg::cs::cartesian> point;
	typedef bg::model::box<point> box;

	class JointNode : public box
	{
	public:
		JointNode(box b);
		bool check = false;
		inline void setUp(JointNode* j)
		{
			up = j;
		}
		inline void setDown(JointNode* j)
		{
			down = j;
		}
		inline void setLeft(JointNode* j)
		{
			left = j;
		}
		inline void setRight(JointNode* j)
		{
			right = j;
		}

		void _line(Mat& img)
		{
			if (check)
				return;
			check = true;
			if (up != nullptr)
			{
				line(img, cv::Point(this->min_corner().get<0>(), this->min_corner().get<1>())
					, cv::Point(up->min_corner().get<0>(), up->min_corner().get<1>()), cv::Scalar(0, 0, 0), 15);
				up->_line(img);
			}
			if (down != nullptr)
			{
				line(img, cv::Point(this->min_corner().get<0>(), this->min_corner().get<1>())
					, cv::Point(down->min_corner().get<0>(), down->min_corner().get<1>()), cv::Scalar(0, 0, 0), 15);
				down->_line(img);
			}
			if (left != nullptr)
			{
				line(img, cv::Point(this->min_corner().get<0>(), this->min_corner().get<1>())
					, cv::Point(left->min_corner().get<0>(), left->min_corner().get<1>()), cv::Scalar(0, 0, 0), 15);
				left->_line(img);
			}
			if (right != nullptr)
			{
				line(img, cv::Point(this->min_corner().get<0>(), this->min_corner().get<1>())
					, cv::Point(right->min_corner().get<0>(), right->min_corner().get<1>()), cv::Scalar(0, 0, 0), 15);
				right->_line(img);
			}
		}
	private:
		JointNode* up;
		JointNode* down;
		JointNode* left;
		JointNode* right;
	};

	class Preprocessor
	{
	public:
		Preprocessor() = delete;
		~Preprocessor() = delete;
		static bool process(PngImage&);

	private:
		static void applyToOrigin(PngImage&, Mat&);
		static void getJoints(PngImage& image, boost::geometry::index::rtree<box, boost::geometry::index::quadratic<16>>& rtree, std::list<JointNode>& l);
	};

}