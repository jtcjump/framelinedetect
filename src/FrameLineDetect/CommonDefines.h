#pragma once
#include <vector>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

enum DOC_TYPE {
	DOC_TYPE_BussinessLicense = 0,
	DOC_TYPE_ForeignersWorkPermit = 1,
	DOC_TYPE_EmploymentPermit = 2,
	DOC_TYPE_ExpertCertificate = 3, 
	DOC_TYPE_HouseholdRegister = 4, 
	DOC_TYPE_BussinessCard = 5,
};

// 直线邻居结构
struct LineNeighbor {
	LineNeighbor() {
		nStartId = -1;
		nEndId = -1;

	}
	int nStartId;
	int nStartScore;

	int nEndId;
	int nEndScore;
};
// 水平线 start 在左侧
// 垂直线 start 在上部
struct LineNode
{
	LineNode() {
		start = Point2f(0.0,0.0);
		end = Point2f(0.0, 0.0);;
	}
	LineNode(Vec4f cvLine){
		start = Point2f(cvLine[0], cvLine[1]);
		end = Point2f(cvLine[2], cvLine[3]);
		
		center.x = fabs(start.x + end.x) / 2;
		center.y = fabs(start.y + end.y) / 2;
	}

	Point2f start;
	Point2f end;
	Point2f center;

	int angle;
	int nDirect; // 0,1,2,3,	左上右下
	int nLength;

	LineNeighbor neighbor;
};

// 框线信息
struct FrameLineNode
{
	FrameLineNode()
	{
		nIdLeft = -1;
		nIdTop = -1;
		nIdRight = -1;
		nIdBottom = -1;
	};
	vector<LineNode> m_vecLines;
	vector<LineNode> m_vecLinesHor;
	vector<LineNode> m_vecLinesVer;

	// 框线在 m_vecLinesHor/ m_vecLinesVer 的id 
	int nIdLeft;
	int nIdTop;
	int nIdRight;
	int nIdBottom;
};

// 框线四边在FrameLineNode:: m_vecLinesHor/m_vecLinesVer 的 Id
struct FrameLineQuadrangle
{
	// 框线id 
	int nIdLeft;
	int nIdTop;
	int nIdRight;
	int nIdBottom;
};
