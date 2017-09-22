#include "stdafx.h"

#include "FrameLineDetect.h"
#include "LineDetect.h"
#include "FrameLineSelect.h"
#include "LineTools.h"



using namespace fld;

#define Max(a,b) (((a) > (b)) ? (a) : (b))
#define Min(a,b) (((a) < (b)) ? (a) : (b))

CFrameLineDetect::CFrameLineDetect():m_fWHRatio_BussinessLicense(0.7), m_nWidthForFLD(720), m_nHeightForFLD(960)
{
}


CFrameLineDetect::~CFrameLineDetect()
{
}

ERROR_FLD CFrameLineDetect::Process(const Mat &img, Mat &img_res, int Param)
{
	if (img.empty())
	{
		return ERROR_IMAGE_INPUT;
	}

	Mat img_gray;
	m_nSrcImgWidth = img.cols;
	m_nSrcImgHeight = img.rows;
	float fScale_x = (float)m_nSrcImgWidth / m_nWidthForFLD;
	m_nHeightForFLD = (float)m_nSrcImgHeight / fScale_x;
	float fScale_y = fScale_x;

	img.copyTo(img_gray);

	if (img.channels()==3)
	{
		cvtColor(img_gray, img_gray, CV_RGB2GRAY);
		resize(img_gray, img_gray, Size(m_nWidthForFLD, m_nHeightForFLD));
	}
	else if (img.channels() ==1)
	{
		resize(img_gray, img_gray, Size(m_nWidthForFLD, m_nHeightForFLD));
	}
	else
	{
		return ERROR_IMAGE_INPUT;
	}

	FrameLineNode frameLineNode;
	if (FrameLineDetect(img_gray, frameLineNode, Param) != 0)
	{
		return ERROR_NO_FRAMELINE;
	}

	// Í¼ÏñÍ¸ÊÓ±ä»»
	Point2f lt, tr, rb, bl;
	lt = CLineTools::GetCrossPoint(frameLineNode.m_vecLinesVer[frameLineNode.nIdLeft], frameLineNode.m_vecLinesHor[frameLineNode.nIdTop]);
	tr = CLineTools::GetCrossPoint(frameLineNode.m_vecLinesHor[frameLineNode.nIdTop], frameLineNode.m_vecLinesVer[frameLineNode.nIdRight]);
	rb = CLineTools::GetCrossPoint(frameLineNode.m_vecLinesVer[frameLineNode.nIdRight], frameLineNode.m_vecLinesHor[frameLineNode.nIdBottom]);
	bl = CLineTools::GetCrossPoint(frameLineNode.m_vecLinesHor[frameLineNode.nIdBottom], frameLineNode.m_vecLinesVer[frameLineNode.nIdLeft]);

	if (Param == DOC_TYPE_BussinessLicense)
	{
		AdjustCornerPoint(lt, tr, rb, bl);
	}

	ConvertPtCoordinate(lt, fScale_x, fScale_y);
	ConvertPtCoordinate(tr, fScale_x, fScale_y);
	ConvertPtCoordinate(rb, fScale_x, fScale_y);
	ConvertPtCoordinate(bl, fScale_x, fScale_y);

#ifdef _DEBUG
	Mat img_forTest;
	img.copyTo(img_forTest);

	line(img_forTest, lt, tr, Scalar(0, 0, 255));
	line(img_forTest, tr, rb, Scalar(0, 0, 255));
	line(img_forTest, rb, bl, Scalar(0, 0, 255));
	line(img_forTest, bl, lt, Scalar(0, 0, 255));
	
	imshow("frame line", img_forTest);
	waitKey(1000);
	imwrite("../log_img/frameline_optimal.jpg", img_forTest);
#endif

	TransformImage(img, lt, tr, rb, bl, img_res);
	return ERROR_NO;
}

ERROR_FLD CFrameLineDetect::Process(const Mat &img, Point2f &ptLeftUp, Point2f &ptRightUp, Point2f &ptRightDown, Point2f &ptLeftDown, int Param)
{
	if (img.empty())
	{
		return ERROR_IMAGE_INPUT;
	}

	Mat img_gray;
	m_nSrcImgWidth = img.cols;
	m_nSrcImgHeight = img.rows;
	float fScale_x = (float)m_nSrcImgWidth / m_nWidthForFLD;
	m_nHeightForFLD = (float)m_nSrcImgHeight / fScale_x;
	float fScale_y = fScale_x;

	img.copyTo(img_gray);

	if (img.channels() == 3)
	{
		cvtColor(img_gray, img_gray, CV_RGB2GRAY);
		resize(img_gray, img_gray, Size(m_nWidthForFLD, m_nHeightForFLD));
	}
	else if (img.channels() == 1)
	{
		resize(img_gray, img_gray, Size(m_nWidthForFLD, m_nHeightForFLD));
	}
	else
	{
		return ERROR_IMAGE_INPUT;
	}

	FrameLineNode frameLineNode;
	if (FrameLineDetect(img_gray, frameLineNode, Param) != 0)
	{
		return ERROR_NO_FRAMELINE;
	}

	ptLeftUp = CLineTools::GetCrossPoint(frameLineNode.m_vecLinesVer[frameLineNode.nIdLeft], frameLineNode.m_vecLinesHor[frameLineNode.nIdTop]);
	ptRightUp = CLineTools::GetCrossPoint(frameLineNode.m_vecLinesHor[frameLineNode.nIdTop], frameLineNode.m_vecLinesVer[frameLineNode.nIdRight]);
	ptRightDown = CLineTools::GetCrossPoint(frameLineNode.m_vecLinesVer[frameLineNode.nIdRight], frameLineNode.m_vecLinesHor[frameLineNode.nIdBottom]);
	ptLeftDown = CLineTools::GetCrossPoint(frameLineNode.m_vecLinesHor[frameLineNode.nIdBottom], frameLineNode.m_vecLinesVer[frameLineNode.nIdLeft]);

	ConvertPtCoordinate(ptLeftUp, fScale_x, fScale_y);
	ConvertPtCoordinate(ptRightUp, fScale_x, fScale_y);
	ConvertPtCoordinate(ptRightDown, fScale_x, fScale_y);
	ConvertPtCoordinate(ptLeftDown, fScale_x, fScale_y);

#ifdef _DEBUG
	Mat img_forTest;
	img.copyTo(img_forTest);

	line(img_forTest, ptLeftUp, ptRightUp, Scalar(0, 0, 255));
	line(img_forTest, ptRightUp, ptRightDown, Scalar(0, 0, 255));
	line(img_forTest, ptRightDown, ptLeftDown, Scalar(0, 0, 255));
	line(img_forTest, ptLeftDown, ptLeftUp, Scalar(0, 0, 255));

	imshow("frame line", img_forTest);
	waitKey(1000);
	imwrite("../log_img/frameline_optimal.jpg", img_forTest);
#endif

	return ERROR_NO;
}

int CFrameLineDetect::FrameLineDetect(const Mat &img_gray, FrameLineNode &frameLineNode, int Param)
{
	int nRet = 0;
	// the line of image detect
	CLineDetect lineDetect;
	vector<Vec4f> lines;
	lineDetect.Process(img_gray, lines);

	// select the frame line (4 lines) from the lines in pre step
	CFrameLineSelect frameLineSelect;
	nRet = frameLineSelect.Process(img_gray, lines, frameLineNode, m_nWidthForFLD, m_nHeightForFLD, Param);

	return nRet;
}

void CFrameLineDetect::AdjustCornerPoint(Point2f &Img_lt, Point2f &Img_tr, Point2f &Img_rb, Point2f &Img_bl)
{
	float fLen1 = 20.0, fLen2 = 40.0;
	
	Img_lt = Point2f(Max(Img_lt.x - fLen1, 0.0), Max(Img_lt.y - fLen2, 0.0));
	Img_tr = Point2f(Min(Img_tr.x + fLen1, (float)m_nWidthForFLD), Max(Img_tr.y - fLen2, 0.0));
	Img_rb = Point2f(Min(Img_rb.x + fLen1, (float)m_nWidthForFLD), Min(Img_rb.y + fLen2, (float)m_nHeightForFLD));
	Img_bl = Point2f(Max(Img_bl.x - fLen1, 0.0), Min(Img_bl.y + fLen2, (float)m_nHeightForFLD));
}

void CFrameLineDetect::TransformImage(const Mat &img_src, Point2f srcImg_lt, Point2f srcImg_tr, Point2f srcImg_rb, Point2f srcImg_bl, Mat &img_res)
{
	int nDstImgWidth, nDstImgHeight;

	EstimateImageInfo(srcImg_lt, srcImg_tr, srcImg_rb, srcImg_bl, nDstImgWidth, nDstImgHeight);

	// lefttop, righttop, leftbottom, rigthtbottom
	Point2f src_vertices[4], dst_vertices[4];
	src_vertices[0] = srcImg_lt;
	src_vertices[1] = srcImg_tr;
	src_vertices[2] = srcImg_bl;
	src_vertices[3] = srcImg_rb;

	dst_vertices[0] = Point(0, 0);
	dst_vertices[1] = Point(nDstImgWidth, 0);
	dst_vertices[2] = Point(0, nDstImgHeight);
	dst_vertices[3] = Point(nDstImgWidth, nDstImgHeight);
	Mat warpMatrix = getPerspectiveTransform(src_vertices, dst_vertices);
	
	warpPerspective(img_src, img_res, warpMatrix, Size(nDstImgWidth, nDstImgHeight), INTER_LINEAR, BORDER_CONSTANT, Scalar(255, 255, 255) );

#ifdef _DEBUG
	Mat img_forTest;
	img_res.copyTo(img_forTest);

	imshow("perspective transform res", img_forTest);
	waitKey(1000);
	imwrite("../log_img/perspective_transform_res.jpg", img_forTest);
#endif
}

void CFrameLineDetect::EstimateImageInfo(const Point2f &srcImg_lt, const Point2f &srcImg_tr, const Point2f & srcImg_rb, const Point2f &srcImg_bl,
	int &dstImg_width, int &dstImg_height)
{
	int nLeftLen, nTopLen, nRightLen, nBottomLen;

	nLeftLen = CLineTools::GetDistance(srcImg_bl, srcImg_lt);
	nTopLen = CLineTools::GetDistance(srcImg_lt, srcImg_tr);
	nRightLen = CLineTools::GetDistance(srcImg_tr, srcImg_rb);
	nBottomLen = CLineTools::GetDistance(srcImg_rb, srcImg_bl);

	dstImg_width = Max(nTopLen, nBottomLen);
	dstImg_height = Max(nLeftLen, nRightLen);
}
