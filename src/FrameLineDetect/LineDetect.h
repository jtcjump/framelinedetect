#pragma once
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#include <vector>

using namespace std;
using namespace cv;

namespace fld
{
	class CLineDetect
	{
	public:
		CLineDetect();
		virtual ~CLineDetect();

	public:
		int Process(const Mat &img_gray, vector<Vec4f> &lines);

	};

}
