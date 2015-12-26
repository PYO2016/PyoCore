#pragma once

#include "../Common/PngImage.h"
#include <list>
#include <memory>
#include <tuple>

namespace TableDetection
{
	enum class HistogramType : int
	{
		TYPE_X = 0,
		TYPE_Y
	};
	enum class ExtremumType : int
	{
		TYPE_MIN = 0,
		TYPE_MAX
	};
	enum class KmeansType : int
	{
		TYPE_LOWER = 0,
		TYPE_UPPER
	};


	class Histogram
	{
	public:
		Histogram(HistogramType type, const Common::PngImage& image, 
			unsigned offsetWidth, unsigned offsetHeight, int length, int valLimit);
		Histogram(const Histogram& h);
		~Histogram();

		bool calculateValues();		// not tested...
		bool applyMedianFilter();	// test pass!
		bool initFilterExtremum();	// not tested
		double getKmeansBoundary(ExtremumType type);
		bool removeKmeansValues(double minBoundary, double maxBoundary);
		
		inline const std::list<std::pair<int, ExtremumType>>& getExtremumList () const;

	private:
		// histogram type ( represents whether x-coordinate or y-coordinate )
		HistogramType type;
		const Common::PngImage& image;
		unsigned offsetWidth, offsetHeight;
		std::vector<int> values;
		int length, valLimit;
		// extremum values
		std::list<std::pair<int, ExtremumType>> extremumList;
	};

	class HistogramManager
	{	
	public:
		HistogramManager(const Common::PngImage& image);
		HistogramManager(const Common::PngImage& image, unsigned areaWidth, unsigned areaHeight, 
			unsigned offsetWidth, unsigned offsetHeight);
		HistogramManager(const HistogramManager& h);
		~HistogramManager();

		void cleanup();
		inline void setAttr(unsigned areaWidth, unsigned areaHeight, 
			unsigned offsetWidth, unsigned offsetHeight);

		bool makeHistogram(HistogramType type);
		bool applyMedianFilter(HistogramType type);
		bool filterExtremum(HistogramType type);
		bool applyKmeans(HistogramType type);
		std::vector<std::tuple<int, int, int, int>> getTableInfo();	// top bottom left right
		/* etc... */

	private:
		const Common::PngImage& image;
		unsigned areaWidth, areaHeight;
		unsigned offsetWidth, offsetHeight;
		std::shared_ptr<Histogram> pHistogramX, pHistogramY;
	};

	/* inline methods */

	inline const std::list<std::pair<int, ExtremumType>>& Histogram::getExtremumList() const
	{
		return this->extremumList;
	}

	inline void HistogramManager::setAttr(unsigned areaWidth, unsigned areaHeight,
		unsigned offsetWidth, unsigned offsetHeight)
	{
		this->areaWidth = areaWidth;
		this->areaHeight = areaHeight;
		this->offsetWidth = offsetWidth;
		this->offsetHeight = offsetHeight;
	}
}