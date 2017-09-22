// Test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>

#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#include <gflags/gflags.h>

#include "../FrameLineDetect/FrameLineDetect.h"

using namespace cv;
using namespace std;
using namespace fld;

DEFINE_int32(doc_type, 0, "the input image type");
DEFINE_string(input_img, "", "the input image path");
DEFINE_string(output_img, "", "the output image path");

int main(int argc, char **argv)
{
	int nRet = 0;
	google::ParseCommandLineFlags(&argc, &argv, true);
	//std::cout << "the input image path:" << FLAGS_input_img << std::endl;
	//std::cout << "the output image path:" << FLAGS_output_img << std::endl;


	// string in = "D:/datasets/bussinesscard/24.jpg";
	
	Mat img_input = imread(FLAGS_input_img, IMREAD_COLOR);
	if (img_input.empty())
	{
		nRet = -1;
	}
	else
	{
		Mat img_res;
		CFrameLineDetect frameLineDetect;
		nRet = frameLineDetect.Process(img_input, img_res, FLAGS_doc_type);

		//Point2f ptLeftUp, ptRightUp, ptRightDown, ptLeftDown;
		//nRet = frameLineDetect.Process(img_input, ptLeftUp, ptRightUp, ptRightDown, ptLeftDown, FLAGS_doc_type);
		if (nRet == 0)
		{
			// 建立路径中不存在得文件夹
			imwrite(FLAGS_output_img, img_res);
		}
		
	}
	
	google::ShutDownCommandLineFlags();
    return nRet;
}

