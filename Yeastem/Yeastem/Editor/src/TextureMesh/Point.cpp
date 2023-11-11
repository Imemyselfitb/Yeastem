#include "yst_pch.h"

#include "Point.h"

YEASTEM_BEGIN

Point Point::Copy()
{
	return { this->x, this->y };
}

void Point::operator+=(const Point& other)
{
	this->x += other.x;
	this->y += other.y;
}

Point Point::operator+(const Point& other)
{
	Point copy = { this->x, this->y };
	copy += other;
	return copy;
}

void Point::operator-=(const Point& other)
{
	this->x -= other.x;
	this->y -= other.y;
}

Point Point::operator-(const Point& other)
{
	Point copy = { this->x, this->y };
	copy -= other;
	return copy;
}

void Point::operator*=(float sf)
{
	this->x *= sf;
	this->y *= sf;
}

Point Point::operator*(float sf)
{
	Point copy = { this->x, this->y };
	copy *= sf;
	return copy;
}

void Point::operator/=(float sf)
{
	this->x /= sf;
	this->y /= sf;
}

Point Point::operator/(float sf)
{
	Point copy = { this->x, this->y };
	copy /= sf;
	return copy;
}

Point Point::Lerp(const Point& a, const Point& b, float t)
{
	return { 
		(a.x + t * (b.x - a.x)), 
		(a.y + t * (b.y - a.y))
	};
}

YEASTEM_END
