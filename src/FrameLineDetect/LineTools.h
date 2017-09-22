#pragma once
#include "CommonDefines.h"
#include <opencv2/opencv.hpp>

using namespace cv;

namespace fld
{
	class CLineTools
	{
	public:
		CLineTools();
		~CLineTools();

		// Ω«∂»
		static void CalcAngle(LineNode &line);

		// æ‡¿Î
		static int GetDistance(const Point2f &ptStart, const Point2f &ptEnd);
		static int GetDistance(const Point2f &pt, const LineNode &line);

		// Ωªµ„
		static Point2f GetCrossPoint(LineNode line_a, LineNode line_b);
		static Point2f GetCrossPoint(Point2f StPt1, Point2f EdPt1, Point2f StPt2, Point2f EdPt2);
	};

}