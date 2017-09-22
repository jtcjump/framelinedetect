#pragma once

#include <opencv2/opencv.hpp>

#include "CommonDefines.h"
#include "Error_FLD.h"

using namespace cv;

namespace fld
{
	class CFrameLineDetect
	{
	public:
		CFrameLineDetect();
		~CFrameLineDetect();

		ERROR_FLD Process(const Mat &img, Mat &img_res, const int Param=0);
		ERROR_FLD Process(const Mat &img, Point2f &ptLeftUp, Point2f &ptRightUp, Point2f &ptRightDown, Point2f &ptLeftDown, const int Param = 0);
	private:
		// 框线检测
		int FrameLineDetect(const Mat &img_gray, FrameLineNode &frameLineNode, const int Param = 0);

		// 图像矫正
		void TransformImage(const Mat &img_src, Point2f srcImg_lt, Point2f srcImg_tr, Point2f srcImg_rb, Point2f srcImg_bl, Mat &img_res);

		// 根据四个点估计原图像信息
		void EstimateImageInfo(const Point2f &srcImg_lt, const Point2f &srcImg_tr, const Point2f & rcImg_rb, const Point2f &srcImg_bl,
			int &dstImg_width, int &dstImg_height);

		// 微调框线四个点
		void AdjustCornerPoint(Point2f &Img_lt, Point2f &Img_tr, Point2f &Img_rb, Point2f &Img_bl);

		// tools
		//
		void ConvertPtCoordinate(Point2f &pt, float scale_x, float scale_y) {
			pt.x = pt.x * scale_x;
			pt.y = pt.y * scale_y;
		}


	private:
		const float m_fWHRatio_BussinessLicense;

		// 框线检测时应用的图像信息
		int m_nWidthForFLD;
		int m_nHeightForFLD;

		// 原图信息
		int m_nSrcImgWidth;
		int m_nSrcImgHeight;

	};

}
