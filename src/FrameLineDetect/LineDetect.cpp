#include "stdafx.h"
#include "LineDetect.h"
#include "CommonDefines.h"

using namespace fld;

CLineDetect::CLineDetect()
{
}


CLineDetect::~CLineDetect()
{
}


int CLineDetect::Process(const Mat &img_gray, vector<Vec4f> &lines)
{
	if (img_gray.empty())
	{
		return -1;
	}

#if 1
	Ptr<LineSegmentDetector> lsd = createLineSegmentDetector(LSD_REFINE_STD);
#else
	Ptr<LineSegmentDetector> lsd = createLineSegmentDetector(LSD_REFINE_NONE);
#endif

	lsd->detect(img_gray, lines);

#ifdef _DEBUG
	// For Debug Show found lines
	Mat drawnLines(img_gray);
	lsd->drawSegments(drawnLines, lines);
	imshow("Standard refinement", drawnLines);
	waitKey(500);
	imwrite("../log_img/lines_lsd.jpg", drawnLines);
#endif
	return 0;
}






