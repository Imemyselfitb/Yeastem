#pragma once

#include "wx/wx.h"

namespace YEASTEM
{
	class Polygon
	{
	public:
		Polygon(wxPoint points[]);
		~Polygon();
		wxPoint points[25] = {};
		int pointsNum = 0;
	};
}