#pragma once

YEASTEM_BEGIN

class Point
{
public:
	Point() {}
	Point(float x, float y) :x(x), y(y) {}
	~Point() {}

public:
	Point Copy();

public:
	void operator+=(const Point& other);
	Point operator+(const Point& other);
	void operator-=(const Point& other);
	Point operator-(const Point& other);
	void operator*=(float sf);
	Point operator*(float sf);
	void operator/=(float sf);
	Point operator/(float sf);

public:
	static Point Lerp(const Point&, const Point&, float);

public:
	float x, y;
};

YEASTEM_END
