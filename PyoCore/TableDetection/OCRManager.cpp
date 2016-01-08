#include "OCRManager.h"
#include <tesseract\baseapi.h>
#include <leptonica\allheaders.h>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <string>
#include <list>
#include <boost/algorithm/string.hpp>

using namespace tesseract;
using namespace Common;

namespace TableDetection
{
	OCRManager::OCRManager()
	{
	}
	
	OCRManager::~OCRManager()
	{	
	}

	bool OCRManager::recognize(PngImage & image, std::list<SparseBlock>& blocks)
	{
		unsigned char* data = image.getDataAsByteArray();
		if (data == nullptr) {
			return false;
		}

		cv::Mat src(image.getHeight(), image.getWidth(), CV_8UC3, data);
		if (src.channels() == 3) {
			cv::cvtColor(src, src, CV_BGR2GRAY);
		}
		
		TessBaseAPI* tessApi = new TessBaseAPI();
		if (tessApi == NULL || tessApi->Init(NULL, "eng")) {
			return false;
		}
		
		unsigned char* imagedata = src.data;
		int width = src.cols;
		int height = src.rows;
		int pixelBytes = src.channels();
		int lineBytes = width * pixelBytes;
		
		tessApi->SetImage(imagedata, width, height, pixelBytes, lineBytes);

		for (auto itr = begin(blocks); itr != end(blocks); ++itr) {
			tessApi->SetRectangle(itr->getLeft(), itr->getTop(), itr->getWidth(), itr->getHeight());
			itr->setText(tessApi->GetUTF8Text());
		}

		tessApi->End();
		return true;
	}

	bool OCRManager::recognize(PngImage & image, std::vector<Cell>& cells)
	{
		std::locale loc;

		unsigned char* data = image.getDataAsByteArray();
		if (data == nullptr) {
			return false;
		}

		cv::Mat src(image.getHeight(), image.getWidth(), CV_8UC3, data);
		if (src.channels() == 3) {
			cv::cvtColor(src, src, CV_BGR2GRAY);
		}

		TessBaseAPI* tessApi = new TessBaseAPI();
		if (tessApi == NULL || tessApi->Init(NULL, "eng", tesseract::OEM_TESSERACT_CUBE_COMBINED)) {
			return false;
		}

		for (auto itr = begin(cells); itr != end(cells); ++itr) {
			cv::Mat sub = src(cv::Rect(itr->getLeft(), itr->getTop(), itr->getWidth(), itr->getHeight())).clone();
			localProcess(sub);
			
			unsigned char* imagedata = sub.data;
			int width = sub.cols;
			int height = sub.rows;
			int pixelBytes = sub.channels();
			int lineBytes = width * pixelBytes;
			
			tessApi->SetImage(imagedata, width, height, pixelBytes, lineBytes);

			std::string utf8(tessApi->GetUTF8Text());
			//std::remove_if(utf8.begin(), utf8.end(), [&](char c)->bool { return !std::isprint(c, loc); });
			//std::replace_if(utf8.begin(), utf8.end(), [&](char c)->bool { return c == '\n'; }, '*');
			boost::replace_all(utf8, "\n", "<br/>");
			std::replace_if(utf8.begin(), utf8.end(), [&](char c)->bool { return !std:: isprint(c, loc); }, ' ');
			itr->setInnerString(std::wstring(utf8.begin(), utf8.end()));
		}

		tessApi->End();
		return true;
	}

	void OCRManager::localProcess(cv::Mat& sub) {

		if (sub.channels() == 3) {
			cv::cvtColor(sub, sub, CV_BGR2GRAY);
		}

		cv::adaptiveThreshold(sub, sub, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 15, 3);
		
		cv::Mat horizontal = sub.clone();
		cv::Mat vertical = sub.clone();

		int ratio = 60;
		int horizontalsize = (horizontal.cols * ratio) / 100;
		cv::Mat horizontalStructure = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(horizontalsize, 1));
		cv::erode(~horizontal, horizontal, horizontalStructure, cv::Point(-1, -1));
		cv::dilate(horizontal, horizontal, horizontalStructure, cv::Point(-1, -1), 2);

		int verticalsize = (vertical.rows * ratio) / 100;
		cv::Mat verticalStructure = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, verticalsize));
		cv::erode(~vertical, vertical, verticalStructure, cv::Point(-1, -1));
		cv::dilate(vertical, vertical, verticalStructure, cv::Point(-1, -1), 2);

		cv::Mat mask = horizontal + vertical;
		sub = sub + mask;
	}
}