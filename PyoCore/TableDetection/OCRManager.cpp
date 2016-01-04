#include "OCRManager.h"
#include <tesseract\baseapi.h>
#include <leptonica\allheaders.h>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <string>

#include <list>


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

	bool OCRManager::recognize(PngImage & image, std::list<SparseBlock>& entries)
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

		for (auto itr = begin(entries); itr != end(entries); ++itr) {
			tessApi->SetRectangle(itr->getLeft(), itr->getTop(), itr->getWidth(), itr->getHeight());
			itr->setText(tessApi->GetUTF8Text());
		}

		tessApi->End();
		return true;
	}

	bool OCRManager::recognize(PngImage & image, std::list<Cell>& entries)
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
		int depth = src.depth();
		int pixelBytes = src.channels() * depth;
		int lineBytes = width * pixelBytes;

		tessApi->SetImage(imagedata, width, height, pixelBytes, lineBytes);

		for (auto itr = begin(entries); itr != end(entries); ++itr) {
			tessApi->SetRectangle(itr->getLeft(), itr->getTop(), itr->getWidth(), itr->getHeight());
			itr->setInnerString(tessApi->GetUTF8Text());
		}

		tessApi->End();
		return true;
	}
}