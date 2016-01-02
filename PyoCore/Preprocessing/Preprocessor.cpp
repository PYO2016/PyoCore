#include "Preprocessor.h"
#include "opencv2\imgproc\imgproc.hpp"
#include "opencv2\highgui\highgui.hpp"

namespace Preprocessing 
{
	bool Preprocessor::process(PngImage& image)
	{
		static int lowThreshold = 30;
		static int ratio = 3;

		Mat src(image.getHeight(), image.getWidth(), CV_8UC3, image.getDataAsByteArray());
		Mat src_gray;
		Mat dst;
		
		// grayscaling
		cvtColor(src, src_gray, CV_BGR2GRAY);

		// Reduce noise with a kernel 3x3
		blur(src_gray, dst, Size(3, 3));
		//medianBlur(src_gray, detected_edges, 5);

		// Canny edge detecting
		Canny(dst, dst, lowThreshold, lowThreshold * ratio, 3);


		// extract the horizontal and vertical lines

		Mat horizontal = dst.clone();
		// Specify size on horizontal axis
		int horizontalsize = horizontal.cols / 30;
		// Create structure element for extracting horizontal lines through morphology operations
		Mat horizontalStructure = getStructuringElement(MORPH_RECT, Size(horizontalsize, 1));
		// Apply morphology operations
		erode(horizontal, horizontal, horizontalStructure, Point(-1, -1));
		dilate(horizontal, horizontal, horizontalStructure, Point(-1, -1));

		Mat vertical = dst.clone();
		// Specify size on vertical axis
		int verticalsize = vertical.rows / 30;
		// Create structure element for extracting vertical lines through morphology operations
		Mat verticalStructure = getStructuringElement(MORPH_RECT, Size(1, verticalsize));
		// Apply morphology operations
		erode(vertical, vertical, verticalStructure, Point(-1, -1));
		dilate(vertical, vertical, verticalStructure, Point(-1, -1));

		// remove horizontal and vertical line from image
		bitwise_and(dst, ~horizontal, dst);
		bitwise_and(dst, ~vertical, dst);

		// reverse black-white
		bitwise_not(dst, dst);

		// change apply to original image
		applyToOrigin(image, dst);

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

				image[i][j].R = intensity;
				image[i][j].G = intensity;
				image[i][j].B = intensity;
			}
		}
	}
}
