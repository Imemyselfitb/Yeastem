#include "yst_pch.h"

#include "Vector2.h"

YEASTEM_BEGIN

void Vector2::set(Vector2 other)
{
	x = other.x;
	y = other.y;
}
void Vector2::set(float xNew, float yNew)
{
	x = xNew;
	y = yNew;
}

Vector2 Vector2::operator+(Vector2 other) const
{
	return { x + other.x, y + other.y };
}
Vector2 Vector2::operator-(Vector2 other) const
{
	return { x - other.x, y - other.y };
}
Vector2 Vector2::operator*(Vector2 other) const
{
	return { x * other.x, y * other.y };
}
Vector2 Vector2::operator/(Vector2 other) const
{
	return { x / other.x, y / other.y };
}
Vector2 Vector2::operator*(float scalar) const
{
	return { x * scalar, y * scalar };
}
Vector2 Vector2::operator/(float scalar) const
{
	return { x / scalar, y / scalar };
}

void Vector2::operator+=(Vector2 other)
{
	x += other.x;
	y += other.y;
}
void Vector2::operator-=(Vector2 other)
{
	x -= other.x;
	y -= other.y;
}
void Vector2::operator*=(Vector2 other)
{
	x *= other.x;
	y *= other.y;
}
void Vector2::operator/=(Vector2 other)
{
	x /= other.x;
	y /= other.y;
}
void Vector2::operator*=(float scalar)
{
	x *= scalar;
	y *= scalar;
}
void Vector2::operator/=(float scalar)
{
	x /= scalar;
	y /= scalar;
}

void Vector2::Rotate(float Angle, Vector2 Pivot)
{
	float s = sin(Angle);
	float c = cos(Angle);

	float xOff = x - Pivot.x;
	float yOff = y - Pivot.y;

	x = (xOff * c) - (yOff * s) + Pivot.x;
	y = (xOff * s) + (yOff * c) + Pivot.y;
}

void Vector2::Scale(float sf, Vector2 centre)
{
	float xOff = x - centre.x;
	float yOff = y - centre.y;

	x = sf * xOff + centre.x;
	y = sf * yOff + centre.y;
}

std::ostream& operator<<(std::ostream& s, const Vector2& v)
{
	s << v.x << ", " << v.y;
	return s;
}

YEASTEM_END

