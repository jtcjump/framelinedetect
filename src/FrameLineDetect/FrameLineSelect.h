#pragma once
#include "CommonDefines.h"
#include <vector>

using namespace std;
namespace fld
{
	class CFrameLineSelect
	{
	public:
		CFrameLineSelect();
		virtual ~CFrameLineSelect();

	public:
		int Process(const Mat &img_gray, vector<Vec4f> &lines, FrameLineNode &frameLine, const int &nWidth, const int &nHeight, const int Param = 0);

	private:

		bool SelectOptimalLine(const Mat &img_gray, FrameLineNode &frameLine);

		// 找线段集合的邻居
		void FindLinesNeighbor(vector<LineNode> &vecLinesHor, vector<LineNode> &vecLinesVer);
		void FindLineNeighbor(LineNode &line, vector<LineNode> &vecLines);

		// 根据每条线段与他的邻居生成框线
		bool CreateFrameLineByNeighbor(const Mat &img_gray, FrameLineNode &frameLine);
		// 根据规则从框线集合vecQuad中挑选最优框线
		int SelectFrameLineFromQuadSet(FrameLineNode &frameLine, const vector<FrameLineQuadrangle> &vecQuad);

		// 判断四边形是否有效
		bool IsValidQuadrangle(const FrameLineNode &frameLine, const FrameLineQuadrangle &flQuad);
		// 判断line_b相对line_a的开始或者结束点是否有效
		bool IsValidLine(const LineNode &line_a, bool IsBeginPoint, const LineNode &line_b);

		// 计算两条直线得分
		int CalcLine2LineScore(const LineNode &line_a, bool IsBeginPoint, const LineNode &line_b);

		// 对检测出的框线进行微调以使用不同类型图像
		void AdjustFrameLine(FrameLineNode &frameLine);

		// 连接与合并直线
		void LinkLineHor(vector<LineNode> &lines);
		void LinkLineVer(vector<LineNode> &lines);

		void MergeLineHor(vector<LineNode> &lines);
		void MergeLineVer(vector<LineNode> &lines);

		// 标记直线信息
		void ClassifyAndMarkLines(vector<Vec4f> &lines, vector<LineNode> &linesHor, vector<LineNode> &linesVer);

		static bool CmpLine_Hor(const LineNode &line_a, const LineNode &line_b);
		static bool CmpLine_Ver(const LineNode &line_a, const LineNode &line_b);

		//tools
		// 计算四个点的外接矩形面积
		int CalcQuadrangleArea(Point2f lt, Point2f tr, Point2f rb, Point2f bl);

	private:
		int m_nImgWidth;
		int m_nImgHeight;

		vector<FrameLineQuadrangle> m_vecQuad;
	};

}