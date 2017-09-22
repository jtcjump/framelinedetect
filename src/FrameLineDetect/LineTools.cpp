#include "stdafx.h"
#include "LineTools.h"


using namespace fld;
CLineTools::CLineTools()
{
}


CLineTools::~CLineTools()
{
}

void CLineTools::CalcAngle(LineNode &line)
{
	int x1 = line.start.x;
	int y1 = line.start.y;
	int x2 = line.end.x;
	int y2 = line.end.y;

	int angle360 = 0;
	if (x1 == x2)
		angle360 = y1 < y2 ? 270 : 90;
	else
	{
		if (y1 == y2)
			angle360 = x1 < x2 ? 0 : 180;
		else
		{
			float dy = y2 - y1;
			float dx = x2 - x1;
			float absangle = atan(fabs(dy) / fabs(dx)) / 3.1415926 * 180;
			if (y2 > y1 && x2 > x1)
				angle360 = 360 - absangle;
			if (y2 < y1 && x2 > x1)
				angle360 = absangle;
			if (y2 > y1 && x2 < x1)
				angle360 = 180 + absangle;
			if (y2 < y1 && x2 < x1)
				angle360 = 180 - absangle;

		}
	}
	line.angle = angle360;
}


int CLineTools::GetDistance(const Point2f &ptStart, const Point2f &ptEnd)
{
	return sqrtf((ptEnd.x - ptStart.x)*(ptEnd.x - ptStart.x) +
		(ptEnd.y - ptStart.y)*(ptEnd.y - ptStart.y));
}

int CLineTools::GetDistance(const Point2f &pt, const LineNode &line)
{
	Point2f ptStart = line.start;
	Point2f ptEnd = line.end;

	int dx = ptEnd.x - ptStart.x;
	int dy = ptEnd.y - ptStart.y;
	if (dx == 0)
		return abs(pt.x - ptStart.x);
	if (dy == 0)
		return abs(pt.y - ptStart.y);
	int distance = abs(pt.y*dx - dy*pt.x + dy*ptStart.x - ptStart.y*dx)*1.0 / (sqrtf(dx*dx + dy*dy));

	return distance;
}

Point2f CLineTools::GetCrossPoint(LineNode line_a, LineNode line_b)
{
	return GetCrossPoint(line_a.start, line_a.end, line_b.start, line_b.end);
}

Point2f CLineTools::GetCrossPoint(Point2f StPt1, Point2f EdPt1, Point2f StPt2, Point2f EdPt2)
{

	Point2f	CrossPnt;
	CrossPnt.x = -1;
	CrossPnt.y = -1;
	double dx, _dx, dy, _dy, x, y;
	dx = EdPt1.x - StPt1.x;
	dy = EdPt1.y - StPt1.y;
	_dx = EdPt2.x - StPt2.x;
	_dy = EdPt2.y - StPt2.y;

	if (_dx*dy == dx*_dy && _dx*dy != 0)	return  CrossPnt;
	if (dx == 0)
	{
		x = StPt1.x;
		y = StPt2.y + (StPt1.x - StPt2.x)*_dy / _dx;
	}
	else
	{
		x = (dx*_dx*(StPt2.y - StPt1.y) - dx*_dy*StPt2.x + _dx*dy*StPt1.x) / (_dx*dy - dx*_dy);
		y = (dy*(x - StPt1.x) + StPt1.y*dx) / dx;
	}

	CrossPnt.x = (x); CrossPnt.y = (y);

	return CrossPnt;
}