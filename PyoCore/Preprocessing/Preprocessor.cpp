#include "Preprocessor.h"
#include "opencv2\imgproc\imgproc.hpp"
#include "opencv2\highgui\highgui.hpp"

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>

#include <list>
#include <queue>

using namespace cv;
using namespace std;
using namespace Common;

namespace Preprocessing
{
	namespace bg = boost::geometry;
	namespace bgi = boost::geometry::index;

	typedef bg::model::point<int, 2, bg::cs::cartesian> point;
	typedef bg::model::box<point> box;

	bool Preprocessor::process(PngImage& image)
	{
		static int lowThreshold = 30;
		static int ratio = 3;

		boost::geometry::index::rtree<box, boost::geometry::index::quadratic<16>> rtree;

		Mat src(image.getHeight(), image.getWidth(), CV_8UC3, image.getDataAsByteArray());

		// Transform source image to gray if it is not
		Mat gray;

		if (src.channels() == 3)
		{
			cvtColor(src, gray, CV_BGR2GRAY);
		}
		else
		{
			gray = src;
		}

		// Show gray image

		// Apply adaptiveThreshold at the bitwise_not of gray, notice the ~ symbol
		Mat bw;
		adaptiveThreshold(~gray, bw, 255, CV_ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 15, -2);

		// Show binary image
		// Create the images that will use to extract the horizonta and vertical lines
		Mat horizontal = bw.clone();
		Mat vertical = bw.clone();

		int scale = 15; // play with this variable in order to increase/decrease the amount of lines to be detected

		// Specify size on horizontal axis
		int horizontalsize = horizontal.cols / scale;

		// Create structure element for extracting horizontal lines through morphology operations
		Mat horizontalStructure = getStructuringElement(MORPH_RECT, Size(horizontalsize, 1));

		// Apply morphology operations
		erode(horizontal, horizontal, horizontalStructure, Point(-1, -1));
		dilate(horizontal, horizontal, horizontalStructure, Point(-1, -1));

		// Show extracted horizontal lines
		// Specify size on vertical axis
		int verticalsize = vertical.rows / scale;

		// Create structure element for extracting vertical lines through morphology operations
		Mat verticalStructure = getStructuringElement(MORPH_RECT, Size(1, verticalsize));

		// Apply morphology operations
		erode(vertical, vertical, verticalStructure, Point(-1, -1));
		dilate(vertical, vertical, verticalStructure, Point(-1, -1));

		// Show extracted vertical lines
		// create a mask which includes the tables
		Mat mask = horizontal + vertical;
		// find the joints between the lines of the tables, we will use this information in order to descriminate tables from pictures (tables will contain more than 4 joints while a picture only 4 (i.e. at the corners))
		Mat joints;
		bitwise_and(horizontal, vertical, joints);

		PngImage p = image;

		applyToOrigin(p, joints);

		std::list<JointNode> l;
		getJoints(p, rtree, l);

		bool deleted = true;
		vector<box> rst;

		for (auto itr = std::begin(l); itr != std::end(l); ++itr)
		{
			//세로
			auto horizontalBox = box(point(itr->min_corner().get<0>(), 0), point(itr->max_corner().get<0>(), image.getHeight()));
			//가로
			auto verticalBox = box(point(0, itr->min_corner().get<1>()), point(image.getWidth(), itr->max_corner().get<1>()));

			rst.clear();
			rtree.query(bgi::intersects(horizontalBox), std::back_inserter(rst));
			std::sort(std::begin(rst), std::end(rst), [](const box& a, const box& b)
			{
				return a.min_corner().get<1>() < b.min_corner().get<1>();
			});
			for (int i = 0; i < rst.size(); i++) {
				if (rst[i].max_corner().get<1>() == itr->max_corner().get<1>() && rst[i].min_corner().get<0>() == itr->min_corner().get<0>()
					&& rst[i].min_corner().get<1>() == itr->min_corner().get<1>() && rst[i].max_corner().get<0>() == itr->max_corner().get<0>())
				{
					if (i != 0)
					{
						for (auto jtr = std::begin(l); jtr != std::end(l); ++jtr)
						{
							if (rst[i - 1].max_corner().get<1>() == jtr->max_corner().get<1>() && rst[i - 1].min_corner().get<0>() == jtr->min_corner().get<0>()
								&& rst[i - 1].min_corner().get<1>() == jtr->min_corner().get<1>() && rst[i - 1].max_corner().get<0>() == jtr->max_corner().get<0>())
							{
								itr->setUp(&(*jtr));
							}
						}
					}
					if (i != rst.size() - 1)
					{
						for (auto jtr = std::begin(l); jtr != std::end(l); ++jtr)
						{
							if (rst[i + 1].max_corner().get<1>() == jtr->max_corner().get<1>() && rst[i + 1].min_corner().get<0>() == jtr->min_corner().get<0>()
								&& rst[i + 1].min_corner().get<1>() == jtr->min_corner().get<1>() && rst[i + 1].max_corner().get<0>() == jtr->max_corner().get<0>())
							{
								itr->setDown(&(*jtr));
							}
						}
					}
				}
			}

			rst.clear();
			rtree.query(bgi::intersects(verticalBox), std::back_inserter(rst));
			std::sort(std::begin(rst), std::end(rst), [](const box& a, const box& b)
			{
				return a.min_corner().get<0>() > b.min_corner().get<1>();
			});

			for (int i = 0; i < rst.size(); i++) {
				if (rst[i].max_corner().get<1>() == itr->max_corner().get<1>() && rst[i].min_corner().get<0>() == itr->min_corner().get<0>()
					&& rst[i].min_corner().get<1>() == itr->min_corner().get<1>() && rst[i].max_corner().get<0>() == itr->max_corner().get<0>())
				{
					if (i != 0)
					{
						for (auto jtr = std::begin(l); jtr != std::end(l); ++jtr)
						{
							if (rst[i - 1].max_corner().get<1>() == jtr->max_corner().get<1>() && rst[i - 1].min_corner().get<0>() == jtr->min_corner().get<0>()
								&& rst[i - 1].min_corner().get<1>() == jtr->min_corner().get<1>() && rst[i - 1].max_corner().get<0>() == jtr->max_corner().get<0>())
							{
								itr->setLeft(&(*jtr));
							}
						}
					}
					if (i != rst.size() - 1)
					{
						for (auto jtr = std::begin(l); jtr != std::end(l); ++jtr)
						{
							if (rst[i + 1].max_corner().get<1>() == jtr->max_corner().get<1>() && rst[i + 1].min_corner().get<0>() == jtr->min_corner().get<0>()
								&& rst[i + 1].min_corner().get<1>() == jtr->min_corner().get<1>() && rst[i + 1].max_corner().get<0>() == jtr->max_corner().get<0>())
							{
								itr->setRight(&(*jtr));
							}
						}
					}
				}
			}
		}

		for (auto& z : l)
			if(!z.check)
				z._line(bw);

		Mat rrr;
		Canny(bw, rrr, 255, 255);

		bitwise_not(rrr, rrr);
		applyToOrigin(image, rrr);

		// for test
		//image.storeToFile(L"C:/Users/KGWANGMIN/Pictures/4_sparse.png");

		return true;
	}
	
	void Preprocessor::applyToOrigin(PngImage& image, Mat& mat)
	{
		// mat is gray image
		int w = image.getWidth();
		int h = image.getHeight();

		for (int i = 0; i < h; ++i) {
			for (int j = 0; j < w; ++j) {
				int intensity = mat.at<uchar>(i, j);

				auto& z = image[i][j];
				bool b = true;
				image[i][j].R = intensity;
				image[i][j].G = intensity;
				image[i][j].B = intensity;
			}
		}
	}

	void Preprocessor::getJoints(PngImage& image, boost::geometry::index::rtree<box, boost::geometry::index::quadratic<16>>& rtree, std::list<JointNode>& l)
	{
		int w = image.getWidth();
		int h = image.getHeight();

		const int directions[8][2] = { { 1, 0 },{ 0, 1 },{ -1, 0 },{ 0, -1 },{ 1, 1 },{ 1, -1 },{ -1, 1 },{ -1, -1 } };
		const int directionsLength = 8;

		bool** isConquered = new bool*[h];

		// all elements set to false
		for (int i = 0; i < h; i++)
			*(isConquered + i) = new bool[w]();

		for (int i = 0; i < h; ++i) {
			for (int j = 0; j < w; ++j) {
				bool b = true;
				if (image[i][j].R == 255
					&& isConquered[i][j] == false)
				{
					int top = i, bottom = i, left = j, right = j;
					std::queue<std::pair<int, int>> q;
					q.emplace(i, j);
					isConquered[i][j] = true;

					while (!q.empty())
					{
						auto elem = q.front();
						q.pop();

						if (elem.first < top)
							top = elem.first;
						else if (bottom < elem.first)
							bottom = elem.first;
						if (elem.second < left)
							left = elem.second;
						else if (right < elem.second)
							right = elem.second;

						for (int i = 0; i < directionsLength; i++)
						{
							int ty = elem.first + directions[i][0];
							int tx = elem.second + directions[i][1];
							if (ty < 0 || tx < 0 || ty >= h|| tx >= w)
								continue;
							if (isConquered[ty][tx] == true || image[ty][tx].R != 255)
								continue;
							isConquered[ty][tx] = true;
							q.emplace(ty, tx);
						}
					}
					rtree.insert(box(point(left, top), point(right, bottom)));
					l.push_back(JointNode(box(point(left, top), point(right, bottom))));
				}
			}
		}

		for (int i = 0; i < h; i++)
			delete [] *(isConquered + i);
		delete [] isConquered;

		return;
	}

	JointNode::JointNode(box b)
		: box(b), up(nullptr), down(nullptr), left(nullptr), right(nullptr), check(false)
	{
	}
}