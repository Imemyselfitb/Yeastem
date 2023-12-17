#include "yst_pch.h"

#include "Vector2.h"

YEASTEM_BEGIN

void Vector2::set(Vector2 other)
{
	this->x = other.x;
	this->y = other.y;
}
void Vector2::set(float x, float y)
{
	this->x = x;
	this->y = y;
}

Vector2 Vector2::operator+(Vector2 other) const
{
	return { this->x + other.x, this->y + other.y };
}
Vector2 Vector2::operator-(Vector2 other) const
{
	return { this->x - other.x, this->y - other.y };
}
Vector2 Vector2::operator*(Vector2 other) const
{
	return { this->x * other.x, this->y * other.y };
}
Vector2 Vector2::operator/(Vector2 other) const
{
	return { this->x / other.x, this->y / other.y };
}
Vector2 Vector2::operator*(float scalar) const
{
	return { this->x * scalar, this->y * scalar };
}
Vector2 Vector2::operator/(float scalar) const
{
	return { this->x / scalar, this->y / scalar };
}

void Vector2::operator+=(Vector2 other)
{
	this->x += other.x;
	this->y += other.y;
}
void Vector2::operator-=(Vector2 other)
{
	this->x -= other.x;
	this->y -= other.y;
}
void Vector2::operator*=(Vector2 other)
{
	this->x *= other.x;
	this->y *= other.y;
}
void Vector2::operator/=(Vector2 other)
{
	this->x /= other.x;
	this->y /= other.y;
}
void Vector2::operator*=(float scalar)
{
	this->x *= scalar;
	this->y *= scalar;
}
void Vector2::operator/=(float scalar)
{
	this->x /= scalar;
	this->y /= scalar;
}

void Vector2::Rotate(float Angle, Vector2 Pivot)
{
	float s = sin(Angle);
	float c = cos(Angle);

	float x = this->x - Pivot.x;
	float y = this->y - Pivot.y;

	this->x = (x * c) - (y * s) + Pivot.x;
	this->y = (x * s) + (y * c) + Pivot.y;
}

std::ostream& operator<<(std::ostream& s, const Vector2& v)
{
	s << v.x << ", " << v.y;
	return s;
}

YEASTEM_END

