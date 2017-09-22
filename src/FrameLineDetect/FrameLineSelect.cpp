#include "stdafx.h"

#include <algorithm>

#include "FrameLineSelect.h"
#include "LineTools.h"


using namespace fld;
using namespace std;

int g_nWPt2Line = 1, g_nWPt2Pt = 5, g_nWAngle = 1;
const int MAX_OFFSET_ANGLE = 5;
const int MIN_LINE_LEN = 100;

CFrameLineSelect::CFrameLineSelect()
{
}


CFrameLineSelect::~CFrameLineSelect()
{
}

int CFrameLineSelect::Process(const Mat &img_gray, vector<Vec4f> &lines, FrameLineNode &frameLine, const int &nWidth, const int &nHeight, const int Param)
{
	int nRet = 0;
	m_nImgWidth = nWidth;
	m_nImgHeight = nHeight;

	vector<LineNode> linesHor, linesVer;

	ClassifyAndMarkLines(lines, linesHor, linesVer);

	sort(linesHor.begin(), linesHor.end(), CmpLine_Hor);
	sort(linesVer.begin(), linesVer.end(), CmpLine_Ver);

	LinkLineHor(linesHor);
	LinkLineVer(linesVer);

	MergeLineHor(linesHor);
	MergeLineVer(linesVer);

	sort(linesHor.begin(), linesHor.end(), CmpLine_Hor);
	sort(linesVer.begin(), linesVer.end(), CmpLine_Ver);

#ifdef _DEBUG
	Scalar arrScalar[3] = { Scalar(255, 0, 0) , Scalar(0, 255, 0) , Scalar(0, 0, 255) };
	imwrite("../log_img/img_gray_forfld.jpg", img_gray);
	Mat img_forTest(img_gray);
	cvtColor(img_forTest, img_forTest, CV_GRAY2BGR);
	vector<LineNode> linesForDebug;
	linesForDebug.insert(linesForDebug.end(), linesHor.begin(), linesHor.end());
	linesForDebug.insert(linesForDebug.end(), linesVer.begin(), linesVer.end());

	for (int i =0; i <linesForDebug.size(); i++)
	{
		line(img_forTest, linesForDebug[i].start, linesForDebug[i].end, arrScalar[i%3]);
	}
	imshow("link the lines", img_forTest);
	waitKey(500);
	imwrite("../log_img/linesLink_lsd.jpg", img_forTest);
#endif

	frameLine.m_vecLinesHor = linesHor;
	frameLine.m_vecLinesVer = linesVer;

	if (!SelectOptimalLine(img_gray, frameLine))
	{
		return 1;
	}

	if (Param == DOC_TYPE_BussinessLicense)
	{
		AdjustFrameLine(frameLine);
	}

	return nRet;
}

//************************************
// Method:    AdjustFrameLine
// Description:  由于存在多个框线 所以需要挑选框线
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: FrameLineNode & frameLine
//************************************
void CFrameLineSelect::AdjustFrameLine(FrameLineNode &frameLine)
{
	const int nThDstX_AdjustLine = 100;
	const int nThDstY_AdjustLine = 150;

	const int nThAngle_AdjustLine = 6;
	const int nThLenRatioLimit = 2;

	vector<LineNode> &vecLinesHor = frameLine.m_vecLinesHor;
	vector<LineNode> &vecLinesVer = frameLine.m_vecLinesVer;

	int &nIdLeft = frameLine.nIdLeft;
	int &nIdTop = frameLine.nIdTop;
	int &nIdRight = frameLine.nIdRight;
	int &nIdBottom = frameLine.nIdBottom;

	// ver
	for (int i =nIdLeft+1; i<vecLinesVer.size(); i++)
	{
		
		if (vecLinesVer[i].nLength * nThLenRatioLimit > vecLinesVer[nIdLeft].nLength &&
			vecLinesVer[i].center.x < m_nImgWidth/2 - 10 &&
			abs(vecLinesVer[nIdLeft].center.x - vecLinesVer[i].center.x) < nThDstX_AdjustLine &&
			(abs(vecLinesVer[nIdLeft].angle % 180 - vecLinesVer[i].angle % 180) < nThAngle_AdjustLine ||
				abs(vecLinesVer[nIdLeft].angle % 180 - vecLinesVer[i].angle % 180) > 180 - nThAngle_AdjustLine))
		{
			nIdLeft = i;
		}
	}

	for (int i = nIdRight - 1; i > 0; i--)
	{

		if (vecLinesVer[i].nLength * nThLenRatioLimit > vecLinesVer[nIdRight].nLength &&
			vecLinesVer[i].center.x > m_nImgWidth / 2 + 10 &&
			abs(vecLinesVer[nIdRight].center.x - vecLinesVer[i].center.x) < nThDstX_AdjustLine &&
			(abs(vecLinesVer[nIdRight].angle % 180 - vecLinesVer[i].angle % 180) < nThAngle_AdjustLine || 
				abs(vecLinesVer[nIdRight].angle % 180 - vecLinesVer[i].angle % 180) > 180 - nThAngle_AdjustLine))
		{
			nIdRight = i;
		}
	}

	// hor
	for (int i = nIdTop + 1; i < vecLinesHor.size(); i++)
	{

		if (vecLinesHor[i].nLength * nThLenRatioLimit > vecLinesHor[nIdTop].nLength &&
			vecLinesHor[i].center.y < m_nImgHeight / 2 - 10 &&
			abs(vecLinesHor[nIdTop].center.y - vecLinesHor[i].center.y) < nThDstY_AdjustLine &&
			(abs(vecLinesHor[nIdTop].angle % 180 - vecLinesHor[i].angle % 180) < nThAngle_AdjustLine || 
				abs(vecLinesHor[nIdTop].angle % 180 - vecLinesHor[i].angle % 180) > 180 - nThAngle_AdjustLine))
		{
			nIdTop = i;
		}
	}

	for (int i = nIdBottom - 1; i > 0; i--)
	{

		if (vecLinesHor[i].nLength * nThLenRatioLimit > vecLinesHor[nIdBottom].nLength &&
			vecLinesHor[i].center.y > m_nImgHeight / 2 + 10 &&
			abs(vecLinesHor[nIdBottom].center.y - vecLinesHor[i].center.y) < nThDstY_AdjustLine &&
			(abs(vecLinesHor[nIdBottom].angle % 180 - vecLinesHor[i].angle % 180) < nThAngle_AdjustLine ||
				abs(vecLinesHor[nIdBottom].angle % 180 - vecLinesHor[i].angle % 180) > 180 - nThAngle_AdjustLine))
		{
			nIdBottom= i;
		}
	}
}

void CFrameLineSelect::LinkLineHor(vector<LineNode> &lines)
{
	int nSize = lines.size();
	int *pFlag = new int[nSize];

	const int MAX_LINE_GAP = 20;
	const int MAX_OFFSETY = 5;

	vector<LineNode> vecLines;
	memset(pFlag, 0, sizeof(int)* nSize);
	for (int i = 0; i < lines.size(); i++)
	{
		if (pFlag[i] == 1)	continue;
		pFlag[i] = 1;

		bool bFlagMerged = true;
		int nCurrDirect = lines[i].nDirect;

		// 沿着结束点方向查找 扩展直线结束方向
		Point2f pt_end = lines[i].end;
		while (bFlagMerged)
		{
			bFlagMerged = false;

			int min_hor_dist = 65535;
			int min_index = -1;
			for (int j = 0; j < lines.size(); j++)
			{
				if (pFlag[j] == 1)	continue;
				LineNode line = lines[j];
				Point2f next_start = lines[j].start;
				int line2lineAngleDis = abs(lines[j].angle % 180 - lines[i].angle % 180) % 180;

				if (fabs(next_start.y - pt_end.y) < MAX_OFFSETY &&
					(line2lineAngleDis <= MAX_OFFSET_ANGLE || line2lineAngleDis >= 180 - MAX_OFFSET_ANGLE))
				{
					int hor_dist = next_start.x - pt_end.x;
					if (hor_dist >= -5 && hor_dist < min_hor_dist)
					{
						min_hor_dist = hor_dist;
						min_index = j;
					}
				}
			}
			if (min_index != -1 && min_hor_dist < MAX_LINE_GAP)
			{
				pt_end = lines[min_index].end;
				pFlag[min_index] = 1;
				bFlagMerged = true;
			}
		}

		// 沿着开始点方向查找 扩展直线开始方向
		bFlagMerged = true;
		Point2f pt_start = lines[i].start;
		while (bFlagMerged)
		{
			bFlagMerged = false;
			int min_hor_dist = 65535;
			int min_index = -1;
			for (int j = 0; j < lines.size(); j++)
			{
				if (pFlag[j] == 1)	continue;
				LineNode line = lines[j];
				Point2f pre_end = lines[j].end;
				int line2lineAngleDis = abs(lines[j].angle % 180 - lines[i].angle % 180) % 180;

				if (fabs(pre_end.y - pt_start.y) < MAX_OFFSETY &&
					(line2lineAngleDis <= MAX_OFFSET_ANGLE || line2lineAngleDis >= 180 - MAX_OFFSET_ANGLE))
				{
					int hor_dist = abs(pt_start.x - pre_end.x);
					if (hor_dist <=5 && hor_dist < min_hor_dist)
					{
						min_hor_dist = hor_dist;
						min_index = j;
					}
				}
			}
			if (min_index != -1 && min_hor_dist < MAX_LINE_GAP)
			{
				pt_start = lines[min_index].start;
				pFlag[min_index] = 1;
				bFlagMerged = true;
			}
		}
		LineNode merged_line;
		merged_line.angle = lines[i].angle;
		merged_line.start = pt_start;
		merged_line.end = pt_end;
		merged_line.center.x = (merged_line.start.x + merged_line.end.x) / 2;
		merged_line.center.y = (merged_line.start.y + merged_line.end.y) / 2;

		int tx = fabs(merged_line.start.x - merged_line.end.x);
		int ty = fabs(merged_line.start.y - merged_line.end.y);
		long tLength = tx*tx + ty*ty;
		merged_line.nLength = tLength;
		merged_line.nDirect = nCurrDirect;

		if (CLineTools::GetDistance(pt_start, pt_end) > MIN_LINE_LEN)
			vecLines.push_back(merged_line);

	}

	lines.clear();
	lines = vecLines;
	delete[] pFlag; pFlag = NULL;
}

void CFrameLineSelect::LinkLineVer(vector<LineNode> &lines)
{
	int nSize = lines.size();
	int *pFlag = new int[nSize];

	const int MAX_LINE_GAP = 20;
	const int MAX_OFFSETY = 5;

	vector<LineNode> vecLines;
	memset(pFlag, 0, sizeof(int)* nSize);
	for (int i = 0; i < lines.size(); i++)
	{
		if (pFlag[i] == 1)	continue;
		pFlag[i] = 1;

		bool bFlagMerged = true;
		int nCurrDirect = lines[i].nDirect;

		// 沿着结束点方向查找 扩展直线结束方向
		Point2f pt_end = lines[i].end;
		while (bFlagMerged)
		{
			bFlagMerged = false;

			int min_hor_dist = 65535;
			int min_index = -1;
			for (int j = 0; j < lines.size(); j++)
			{
				if (pFlag[j] == 1)	continue;
				LineNode line = lines[j];
				Point2f next_start = lines[j].start;
				int line2lineAngleDis = abs(lines[j].angle % 180 - lines[i].angle % 180) % 180;

				if (fabs(next_start.x - pt_end.x) < MAX_OFFSETY &&
					(line2lineAngleDis <= MAX_OFFSET_ANGLE || line2lineAngleDis >= 180 - MAX_OFFSET_ANGLE))
				{
					int hor_dist = next_start.y - pt_end.y;
					if (hor_dist >= -5 && hor_dist < min_hor_dist)
					{
						min_hor_dist = hor_dist;
						min_index = j;
					}
				}
			}
			if (min_index != -1 && min_hor_dist < MAX_LINE_GAP)
			{
				pt_end = lines[min_index].end;
				pFlag[min_index] = 1;
				bFlagMerged = true;
			}
		}

		// 沿着开始点方向查找 扩展直线开始方向
		bFlagMerged = true;
		Point2f pt_start = lines[i].start;
		while (bFlagMerged)
		{
			bFlagMerged = false;
			int min_hor_dist = 65535;
			int min_index = -1;
			for (int j = 0; j < lines.size(); j++)
			{
				if (pFlag[j] == 1)	continue;
				LineNode line = lines[j];
				Point2f pre_end = lines[j].end;
				int line2lineAngleDis = abs(lines[j].angle % 180 - lines[i].angle % 180) % 180;

				if (fabs(pre_end.x - pt_start.x) < MAX_OFFSETY &&
					(line2lineAngleDis <= MAX_OFFSET_ANGLE || line2lineAngleDis >= 180 - MAX_OFFSET_ANGLE))
				{
					int hor_dist = pt_start.y - pre_end.y;
					if (hor_dist >= -5 && hor_dist < min_hor_dist)
					{
						min_hor_dist = hor_dist;
						min_index = j;
					}
				}
			}
			if (min_index != -1 && min_hor_dist < MAX_LINE_GAP)
			{
				pt_start = lines[min_index].start;
				pFlag[min_index] = 1;
				bFlagMerged = true;
			}
		}
		LineNode merged_line;
		merged_line.angle = lines[i].angle;
		merged_line.start = pt_start;
		merged_line.end = pt_end;
		merged_line.center.x = (merged_line.start.x + merged_line.end.x) / 2;
		merged_line.center.y = (merged_line.start.y + merged_line.end.y) / 2;

		int tx = fabs(merged_line.start.x - merged_line.end.x);
		int ty = fabs(merged_line.start.y - merged_line.end.y);
		long tLength = tx*tx + ty*ty;
		merged_line.nLength = tLength;
		merged_line.nDirect = nCurrDirect;

		if (CLineTools::GetDistance(pt_start, pt_end) > MIN_LINE_LEN)
			vecLines.push_back(merged_line);

	}

	lines.clear();
	lines = vecLines;
	delete[] pFlag; pFlag = NULL;
}

void CFrameLineSelect::MergeLineHor(vector<LineNode> &lines)
{
	int nSize = lines.size();
	if (nSize < 3)
	{
		return;
	}

	const int MAX_LINE_GAP = 6;
	bool bFlagMerged = true;

	while (bFlagMerged)
	{
		bFlagMerged = false;
		for (int i = 0; i < nSize; i++)
		{
			for (int j = 0; j < nSize; j++)
			{
				if (i == j) continue;

				int line2lineAngleDis = abs(lines[j].angle % 180 - lines[i].angle % 180) % 180;
				int line2lineLenDis = CLineTools::GetDistance(lines[i].center, lines[j]);

				if ( (line2lineAngleDis <= MAX_OFFSET_ANGLE || line2lineAngleDis >= 180 - MAX_OFFSET_ANGLE) &&
					line2lineLenDis < MAX_LINE_GAP)
				{
					// merge line
					bFlagMerged = true;

					lines[i].start = lines[i].start.x > lines[j].start.x ? lines[j].start : lines[i].start;
					lines[i].end = lines[i].end.x > lines[j].end.x ? lines[i].end : lines[j].end;

					lines[i].center.x = (lines[i].start.x + lines[i].end.x) / 2;
					lines[i].center.y = (lines[i].start.y + lines[i].end.y) / 2;

					int tx = fabs(lines[i].start.x - lines[i].end.x);
					int ty = fabs(lines[i].start.y - lines[i].end.y);
					long tLength = tx*tx + ty*ty;
					lines[i].nLength = tLength;

					lines[j] = lines[nSize - 1];
					nSize--;
				}
			}
		}
	}
	lines.erase(lines.begin() + nSize, lines.end());
}

void CFrameLineSelect::MergeLineVer(vector<LineNode> &lines)
{
	int nSize = lines.size();
	if (nSize < 3)
	{
		return;
	}

	const int MAX_LINE_GAP = 6;

	bool bFlagMerged = true;
	while (bFlagMerged)
	{
		bFlagMerged = false;

		for (int i = 0; i < nSize; i++)
		{
			for (int j = 0; j < nSize; j++)
			{
				if (i == j) continue;

				int line2lineAngleDis = abs(lines[j].angle % 180 - lines[i].angle % 180) % 180;
				int line2lineLenDis = CLineTools::GetDistance(lines[i].center, lines[j]);

				if ((line2lineAngleDis <= MAX_OFFSET_ANGLE || line2lineAngleDis >= 180 - MAX_OFFSET_ANGLE) &&
					line2lineLenDis < MAX_LINE_GAP)
				{
					// merge line
					bFlagMerged = true;

					lines[i].start = lines[i].start.y > lines[j].start.y ? lines[j].start : lines[i].start;
					lines[i].end = lines[i].end.y > lines[j].end.y ? lines[i].end : lines[j].end;

					lines[i].center.x = (lines[i].start.x + lines[i].end.x) / 2;
					lines[i].center.y = (lines[i].start.y + lines[i].end.y) / 2;

					int tx = fabs(lines[i].start.x - lines[i].end.x);
					int ty = fabs(lines[i].start.y - lines[i].end.y);
					long tLength = tx*tx + ty*ty;
					lines[i].nLength = tLength;

					lines[j] = lines[nSize - 1];
					nSize--;
				}
			}
		}
	}

	lines.erase(lines.begin() + nSize, lines.end());

}

//************************************
// Method:    SelectOptimalLine
// Description:  寻找每一条直线的最优邻居
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: FrameLineNode & frameLine
//************************************
bool CFrameLineSelect::SelectOptimalLine(const Mat &img_gray, FrameLineNode &frameLine)
{
	bool bRet =true;
	vector<LineNode> &vecLinesHor = frameLine.m_vecLinesHor;
	vector<LineNode> &vecLinesVer = frameLine.m_vecLinesVer;

	FindLinesNeighbor(vecLinesHor, vecLinesVer);

	bRet = CreateFrameLineByNeighbor(img_gray, frameLine);

	return bRet;
}

void CFrameLineSelect::FindLinesNeighbor(vector<LineNode> &vecLinesHor, vector<LineNode> &vecLinesVer)
{
	
	for (int i =0; i<vecLinesHor.size(); i++)
	{
		FindLineNeighbor(vecLinesHor[i], vecLinesVer);
	}

	for (int i =0; i<vecLinesVer.size(); i++)
	{
		FindLineNeighbor(vecLinesVer[i], vecLinesHor);
	}

}

void CFrameLineSelect::FindLineNeighbor(LineNode &line, vector<LineNode> &vecLines)
{
	// line开始点
	int nMaxScore = -1;
	int nMaxId = -1;
	for (int i = 0; i < vecLines.size(); i++)
	{
		
		if (IsValidLine(line, 1, vecLines[i]));
		{
			int nScore = CalcLine2LineScore(line, 1, vecLines[i]);
			if (nScore > nMaxScore)
			{
				nMaxScore = nScore;
				nMaxId = i;
			}
		}
	}
	line.neighbor.nStartId = nMaxId;
	line.neighbor.nStartScore = nMaxScore;

	// line结束点
	nMaxScore = -1;
	nMaxId = -1;
	for (int i = 0; i < vecLines.size(); i++)
	{
		if (IsValidLine(line, 0, vecLines[i]));
		{
			int nScore = CalcLine2LineScore(line, 0, vecLines[i]);
			if (nScore > nMaxScore)
			{
				nMaxScore = nScore;
				nMaxId = i;
			}
		}
	}
	line.neighbor.nEndId = nMaxId;
	line.neighbor.nEndScore = nMaxScore;

}

bool CFrameLineSelect::CreateFrameLineByNeighbor(const Mat &img_gray, FrameLineNode &frameLine)
{
	
	vector<LineNode> &vecLinesHor = frameLine.m_vecLinesHor;
	vector<LineNode> &vecLinesVer = frameLine.m_vecLinesVer;

	for (int i =0; i<vecLinesHor.size();i++)
	{
		LineNode line = vecLinesHor[i];
		FrameLineQuadrangle tQuad;
		if (line.nDirect == 1)
		{
			if (line.neighbor.nEndId !=-1 && line.neighbor.nStartId != -1 &&
				i == vecLinesVer[line.neighbor.nEndId].neighbor.nStartId &&
				i == vecLinesVer[line.neighbor.nStartId].neighbor.nStartId)
			{
				tQuad.nIdTop = i;
				tQuad.nIdLeft = line.neighbor.nStartId;
				tQuad.nIdRight = line.neighbor.nEndId;

				LineNode lineLeftVer = vecLinesVer[line.neighbor.nStartId];
				LineNode lineRightVer = vecLinesVer[line.neighbor.nEndId];
				if (lineLeftVer.neighbor.nEndId !=-1 && lineRightVer.neighbor.nEndId !=-1)
				{
					tQuad.nIdBottom = lineLeftVer.neighbor.nEndScore > lineRightVer.neighbor.nEndScore ? lineLeftVer.neighbor.nEndId : lineRightVer.neighbor.nEndId;

					m_vecQuad.push_back(tQuad);
				}
				else if (lineLeftVer.neighbor.nEndId != -1 || lineRightVer.neighbor.nEndId != -1)
				{
					tQuad.nIdBottom = lineLeftVer.neighbor.nEndId != -1 ? lineLeftVer.neighbor.nEndId : lineRightVer.neighbor.nEndId;

					m_vecQuad.push_back(tQuad);
				}
			}
		}
		else if (line.nDirect == 3)
		{
			if (line.neighbor.nEndId != -1 && line.neighbor.nStartId != -1 &&
				i == vecLinesVer[line.neighbor.nEndId].neighbor.nEndId &&
				i == vecLinesVer[line.neighbor.nStartId].neighbor.nEndId)
			{
				tQuad.nIdBottom = i;
				tQuad.nIdLeft = line.neighbor.nStartId;
				tQuad.nIdRight = line.neighbor.nEndId;

				LineNode lineLeftVer = vecLinesVer[line.neighbor.nStartId];
				LineNode lineRightVer = vecLinesVer[line.neighbor.nEndId];
				if (lineLeftVer.neighbor.nStartId != -1 && lineRightVer.neighbor.nStartId != -1)
				{
					tQuad.nIdTop = lineLeftVer.neighbor.nStartScore > lineRightVer.neighbor.nStartScore ? lineLeftVer.neighbor.nStartId : lineRightVer.neighbor.nStartId;

					m_vecQuad.push_back(tQuad);
				}
				else if (lineLeftVer.neighbor.nStartId != -1 || lineRightVer.neighbor.nStartId != -1)
				{
					tQuad.nIdTop = lineLeftVer.neighbor.nStartId != -1 ? lineLeftVer.neighbor.nStartId : lineRightVer.neighbor.nStartId;

					m_vecQuad.push_back(tQuad);
				}
			}
		}
	}

	for (int i = 0; i < vecLinesVer.size();i++)
	{
		LineNode line = vecLinesVer[i];
		FrameLineQuadrangle tQuad;
		if (line.nDirect == 0)
		{
			if (line.neighbor.nEndId != -1 && line.neighbor.nStartId != -1 &&
				i == vecLinesHor[line.neighbor.nEndId].neighbor.nStartId &&
				i == vecLinesHor[line.neighbor.nStartId].neighbor.nStartId)
			{
				tQuad.nIdLeft = i;
				tQuad.nIdTop = line.neighbor.nStartId;
				tQuad.nIdBottom = line.neighbor.nEndId;

				LineNode lineTopHor = vecLinesHor[line.neighbor.nStartId];
				LineNode lineBottomHor = vecLinesHor[line.neighbor.nEndId];
				if (lineTopHor.neighbor.nEndId != -1 && lineBottomHor.neighbor.nEndId != -1)
				{
					tQuad.nIdRight = lineTopHor.neighbor.nEndScore > lineBottomHor.neighbor.nEndScore ? lineTopHor.neighbor.nEndId : lineBottomHor.neighbor.nEndId;

					m_vecQuad.push_back(tQuad);
				}
				else if (lineTopHor.neighbor.nEndId != -1 || lineBottomHor.neighbor.nEndId != -1)
				{
					tQuad.nIdRight = lineTopHor.neighbor.nEndId != -1 ? lineTopHor.neighbor.nEndId : lineBottomHor.neighbor.nEndId;

					m_vecQuad.push_back(tQuad);
				}
			}
		}
		else if (line.nDirect == 2)
		{
			if (line.neighbor.nEndId != -1 && line.neighbor.nStartId != -1 &&
				i == vecLinesHor[line.neighbor.nEndId].neighbor.nEndId &&
				i == vecLinesHor[line.neighbor.nStartId].neighbor.nEndId)
			{
				tQuad.nIdRight = i;
				tQuad.nIdTop = line.neighbor.nStartId;
				tQuad.nIdBottom = line.neighbor.nEndId;

				LineNode lineTopHor = vecLinesHor[line.neighbor.nStartId];
				LineNode lineBottomHor = vecLinesHor[line.neighbor.nEndId];
				if (lineTopHor.neighbor.nStartId != -1 && lineBottomHor.neighbor.nStartId != -1)
				{
					tQuad.nIdLeft = lineTopHor.neighbor.nStartScore > lineBottomHor.neighbor.nStartScore ? lineTopHor.neighbor.nStartId : lineBottomHor.neighbor.nStartId;

					m_vecQuad.push_back(tQuad);
				}
				else if (lineTopHor.neighbor.nStartId != -1 || lineBottomHor.neighbor.nStartId != -1)
				{
					tQuad.nIdLeft = lineTopHor.neighbor.nStartId != -1 ? lineTopHor.neighbor.nStartId : lineBottomHor.neighbor.nStartId;

					m_vecQuad.push_back(tQuad);
				}
			}
		}
	}

#ifdef _DEBUG
	Mat img_forTest(img_gray);
	cvtColor(img_forTest, img_forTest, COLOR_GRAY2BGR);
	Scalar arrScalar[3] = { Scalar(255, 0, 0) , Scalar(0, 255, 0) , Scalar(0, 0, 255) };
	for (int i = 0; i < m_vecQuad.size(); i++)
	{
		Point2f lt, tr, rb, bl;
		lt = CLineTools::GetCrossPoint(frameLine.m_vecLinesVer[m_vecQuad[i].nIdLeft],		frameLine.m_vecLinesHor[m_vecQuad[i].nIdTop]);
		tr = CLineTools::GetCrossPoint(frameLine.m_vecLinesHor[m_vecQuad[i].nIdTop],		frameLine.m_vecLinesVer[m_vecQuad[i].nIdRight]);
		rb = CLineTools::GetCrossPoint(frameLine.m_vecLinesVer[m_vecQuad[i].nIdRight],	frameLine.m_vecLinesHor[m_vecQuad[i].nIdBottom]);
		bl = CLineTools::GetCrossPoint(frameLine.m_vecLinesHor[m_vecQuad[i].nIdBottom],	frameLine.m_vecLinesVer[m_vecQuad[i].nIdLeft]);

		line(img_forTest, lt, tr, arrScalar[i % 3]);
		line(img_forTest, tr, rb, arrScalar[i % 3]);
		line(img_forTest, rb, bl, arrScalar[i % 3]);
		line(img_forTest, bl, lt, arrScalar[i % 3]);

	}
	imshow("link the lines", img_forTest);
	waitKey(500);
	imwrite("../log_img/framelinesets.jpg", img_forTest);
#endif

	int nId = SelectFrameLineFromQuadSet(frameLine, m_vecQuad);

	if (nId >=0)
	{
		frameLine.nIdLeft = m_vecQuad[nId].nIdLeft;
		frameLine.nIdTop = m_vecQuad[nId].nIdTop;
		frameLine.nIdRight = m_vecQuad[nId].nIdRight;
		frameLine.nIdBottom = m_vecQuad[nId].nIdBottom;

		return true;
	}
	else
	{
		frameLine.nIdLeft = -1;
		frameLine.nIdTop = -1;
		frameLine.nIdRight = -1;
		frameLine.nIdBottom = -1;

		return false;
	}

	return false;
}

int CFrameLineSelect::SelectFrameLineFromQuadSet(FrameLineNode &frameLine, const vector<FrameLineQuadrangle> &vecQuad)
{

	vector<LineNode> &vecLinesHor = frameLine.m_vecLinesHor;
	vector<LineNode> &vecLinesVer = frameLine.m_vecLinesVer;

	const int nThArea = m_nImgHeight*m_nImgWidth >> 2;
	int nMaxArea = -1;
	int nMaxAreaId = -1;
	for (int i =0; i<vecQuad.size(); i++)
	{
		if (IsValidQuadrangle(frameLine, vecQuad[i]))
		{
			Point2f lt, tr, rb, bl;
			lt = CLineTools::GetCrossPoint(vecLinesVer[vecQuad[i].nIdLeft], vecLinesHor[vecQuad[i].nIdTop]);
			tr = CLineTools::GetCrossPoint(vecLinesHor[vecQuad[i].nIdTop], vecLinesVer[vecQuad[i].nIdRight]);
			rb = CLineTools::GetCrossPoint(vecLinesVer[vecQuad[i].nIdRight], vecLinesHor[vecQuad[i].nIdBottom]);
			bl = CLineTools::GetCrossPoint(vecLinesHor[vecQuad[i].nIdBottom], vecLinesVer[vecQuad[i].nIdLeft]);

			int nArea = CalcQuadrangleArea(lt, tr, rb, bl);

			if (nArea >nThArea && 
				nArea > nMaxArea)
			{
				nMaxArea = nArea;
				nMaxAreaId = i;
			}
		}
	}

	return nMaxAreaId;
}

bool CFrameLineSelect::IsValidQuadrangle(const FrameLineNode &frameLine, const FrameLineQuadrangle &flQuad)
{
	LineNode leftLine, topLine, rightLine, bottomLine;
	leftLine = frameLine.m_vecLinesVer[flQuad.nIdLeft];
	topLine = frameLine.m_vecLinesHor[flQuad.nIdTop];
	rightLine = frameLine.m_vecLinesVer[flQuad.nIdRight];
	bottomLine = frameLine.m_vecLinesHor[flQuad.nIdBottom];

	const int nOverlop = 50;
	if (leftLine.center.x - nOverlop <m_nImgWidth/2 &&
		rightLine.center.x + nOverlop >m_nImgWidth/2 &&
		topLine.center.y - nOverlop <m_nImgHeight/2 &&
		bottomLine.center.y + nOverlop >m_nImgHeight/2)
	{
		return true;
	}

	return false;
}

//************************************
// Method:    IsValidLine
// Description:  判断直线lina_a 的开始点或者结束点与直线Line_b的位置关系是否有效
// Access:    private 
// Returns:   bool
// Qualifier:
// Parameter: const LineNode & line_a
// Parameter: bool IsBeginPoint
// Parameter: const LineNode & line_b
//************************************
bool CFrameLineSelect::IsValidLine(const LineNode &line_a, bool IsBeginPoint, const LineNode &line_b)
{
	const int nOverlop = 5;
	if (IsBeginPoint)
	{
		Point2f pt = line_a.start;
		if (line_a.nDirect == 0)
		{
			if (pt.x - nOverlop < line_b.start.x &&
				pt.y + nOverlop > line_b.start.y)
			{
				return true;
			}
		}
		else if (line_a.nDirect == 1)
		{
			if (pt.x + nOverlop > line_b.start.x &&
				pt.y - nOverlop < line_b.start.y)
			{
				return true;
			}
		}
		else if (line_a.nDirect == 2)
		{
			if (pt.x + nOverlop > line_b.end.x &&
				pt.y + nOverlop > line_b.end.y)
			{
				return true;
			}
		}
		else if (line_a.nDirect == 3)
		{
			if (pt.x + nOverlop > line_b.end.x &&
				pt.y + nOverlop > line_b.end.y)
			{
				return true;
			}
		}
	}
	else
	{
		Point2f pt = line_a.end;
		if (line_a.nDirect == 0)
		{
			if (pt.x - nOverlop < line_b.start.x &&
				pt.y - nOverlop < line_b.start.y)
			{
				return true;
			}
		}
		else if (line_a.nDirect == 1)
		{
			if (pt.x - nOverlop < line_b.start.x &&
				pt.y - nOverlop < line_b.start.y)
			{
				return true;
			}
		}
		else if (line_a.nDirect == 2)
		{
			if (pt.x + nOverlop > line_b.end.x &&
				pt.y - nOverlop < line_b.end.y)
			{
				return true;
			}
		}
		else if (line_a.nDirect == 3)
		{
			if (pt.x - nOverlop < line_b.end.x &&
				pt.y + nOverlop > line_b.end.y)
			{
				return true;
			}
		}
	}
	return false;
}

//************************************
// Method:    CalcLine2LineScore
// Description:  计算直线 a 与 b 的得分，依据特征： 距离，角度
// Access:    private 
// Returns:   int
// Qualifier:
// Parameter: const LineNode & line_a
// Parameter: bool IsBeginPoint
// Parameter: const LineNode & line_b
//************************************
int CFrameLineSelect::CalcLine2LineScore(const LineNode &line_a, bool IsBeginPoint, const LineNode &line_b)
{
	int nScore =0;
	int nDstPt2Line = 65535;
	int nDstPt2Pt = 65535;
	int nDisAngle = 360;

	int nScorePt2Line = 0;
	int nScorePt2Pt = 0;
	int nScoreAngle = 0;


	if (IsBeginPoint)
	{
		if (line_a.nDirect == 0 || line_a.nDirect == 1)
		{
			nDstPt2Pt = CLineTools::GetDistance(line_a.start, line_b.start);
		}
		else if (line_a.nDirect == 2 || line_a.nDirect == 3)
		{
			nDstPt2Pt = CLineTools::GetDistance(line_a.start, line_b.end);
		}
		nDstPt2Line = CLineTools::GetDistance(line_a.start, line_b);
	}
	else
	{
		if (line_a.nDirect == 0 || line_a.nDirect == 1)
		{
			nDstPt2Pt = CLineTools::GetDistance(line_a.end, line_b.start);

		}
		else if (line_a.nDirect == 2 || line_a.nDirect == 3)
		{
			nDstPt2Pt = CLineTools::GetDistance(line_a.end, line_b.end);
		}
		nDstPt2Line = CLineTools::GetDistance(line_a.end, line_b);
	}

	nDisAngle = abs(line_a.angle % 90 - line_b.angle % 90);

	nScorePt2Line = nDstPt2Line==0 ? 100 : -nDstPt2Pt / 2 + 100;
	nScorePt2Pt = nDstPt2Pt <=5 ? 100 : -nDstPt2Pt/2 +100;
	nScoreAngle = nDisAngle == 0 ? 100 : (100 - nDisAngle);

	nScorePt2Line = 0;
	nScoreAngle = 0;
	nScore = g_nWPt2Line*nScorePt2Line + g_nWPt2Pt*nScorePt2Pt + g_nWAngle*nScoreAngle;
	return nScore;
}

void CFrameLineSelect::ClassifyAndMarkLines(vector<Vec4f> &lines, vector<LineNode> &linesHor, vector<LineNode> &linesVer)
{
	const int nLop_Angle = 5;

	for (int i = 0; i < lines.size(); i++)
	{
		LineNode line = LineNode(lines[i]);
		
		CLineTools::CalcAngle(line);

		int x1 = line.start.x;
		int y1 = line.start.y;
		int x2 = line.end.x;
		int y2 = line.end.y;

		int angle360 = line.angle;
		
		if ((angle360 > 315 + nLop_Angle || angle360 < 45 - nLop_Angle) ||
			(angle360 > 135 + nLop_Angle && angle360 < 225 - nLop_Angle))
		{
			if (x1 > x2)
			{
				line.start.x = x2;
				line.start.y = y2;
				line.end.x = x1;
				line.end.y = y1;

			}

			line.center.y < m_nImgHeight / 2 ? line.nDirect = 1 : line.nDirect = 3;		
			linesHor.push_back(line);
		}
		if ((angle360 > 45 + nLop_Angle && angle360 < 135 - nLop_Angle) ||
			(angle360 > 225 + nLop_Angle && angle360 < 315 - nLop_Angle))
		{
			if (y1 > y2)
			{
				line.start.x = x2;
				line.start.y = y2;
				line.end.x = x1;
				line.end.y = y1;

			}

			line.center.x < m_nImgWidth / 2 ? line.nDirect = 0 : line.nDirect = 2;
			linesVer.push_back(line);
		}
	}

}

bool CFrameLineSelect::CmpLine_Hor(const LineNode &line_a, const LineNode &line_b)
{
	return line_a.center.y < line_b.center.y;
}

bool CFrameLineSelect::CmpLine_Ver(const LineNode &line_a, const LineNode &line_b)
{
	return line_a.center.x < line_b.center.x;
}

int CFrameLineSelect::CalcQuadrangleArea(Point2f lt, Point2f tr, Point2f rb, Point2f bl)
{
	int nArea = 0;
	Point2f ptLT, ptRB;

	ptLT.x = min(lt.x, bl.x);
	ptLT.y = min(lt.y, tr.y);

	ptRB.x = max(rb.x, tr.x);
	ptRB.y = max(rb.y, bl.y);

	nArea = abs(ptRB.x - ptLT.x)*abs(ptRB.y - ptLT.y);
	return nArea;
}
