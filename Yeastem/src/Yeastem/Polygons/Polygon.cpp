#include "Polygon.h"

namespace YEASTEM {
	Polygon::Polygon(wxPoint points[])
	{
		std::vector<wxPoint> points;

		this.points = points;
		this.pointsNum++;
	}

	void show(wxPaintDC& dc)
	{
		
	}

	Polygon::~Polygon()
	{ }
}