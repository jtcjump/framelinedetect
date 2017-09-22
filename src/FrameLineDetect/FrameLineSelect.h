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

		// ���߶μ��ϵ��ھ�
		void FindLinesNeighbor(vector<LineNode> &vecLinesHor, vector<LineNode> &vecLinesVer);
		void FindLineNeighbor(LineNode &line, vector<LineNode> &vecLines);

		// ����ÿ���߶��������ھ����ɿ���
		bool CreateFrameLineByNeighbor(const Mat &img_gray, FrameLineNode &frameLine);
		// ���ݹ���ӿ��߼���vecQuad����ѡ���ſ���
		int SelectFrameLineFromQuadSet(FrameLineNode &frameLine, const vector<FrameLineQuadrangle> &vecQuad);

		// �ж��ı����Ƿ���Ч
		bool IsValidQuadrangle(const FrameLineNode &frameLine, const FrameLineQuadrangle &flQuad);
		// �ж�line_b���line_a�Ŀ�ʼ���߽������Ƿ���Ч
		bool IsValidLine(const LineNode &line_a, bool IsBeginPoint, const LineNode &line_b);

		// ��������ֱ�ߵ÷�
		int CalcLine2LineScore(const LineNode &line_a, bool IsBeginPoint, const LineNode &line_b);

		// �Լ����Ŀ��߽���΢����ʹ�ò�ͬ����ͼ��
		void AdjustFrameLine(FrameLineNode &frameLine);

		// ������ϲ�ֱ��
		void LinkLineHor(vector<LineNode> &lines);
		void LinkLineVer(vector<LineNode> &lines);

		void MergeLineHor(vector<LineNode> &lines);
		void MergeLineVer(vector<LineNode> &lines);

		// ���ֱ����Ϣ
		void ClassifyAndMarkLines(vector<Vec4f> &lines, vector<LineNode> &linesHor, vector<LineNode> &linesVer);

		static bool CmpLine_Hor(const LineNode &line_a, const LineNode &line_b);
		static bool CmpLine_Ver(const LineNode &line_a, const LineNode &line_b);

		//tools
		// �����ĸ������Ӿ������
		int CalcQuadrangleArea(Point2f lt, Point2f tr, Point2f rb, Point2f bl);

	private:
		int m_nImgWidth;
		int m_nImgHeight;

		vector<FrameLineQuadrangle> m_vecQuad;
	};

}