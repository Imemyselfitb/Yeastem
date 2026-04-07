#include "yst_pch.h"

#include "MathWrapper.h"

YEASTEM_BEGIN

Vector2f Vector2f::Rotated(float angle) const
{
	float c = cos(angle);
	float s = sin(angle);
	return Vector2f(
		x * c - y * s,
		x * s + y * c
	);
}

std::ostream& operator<<(std::ostream& s, const Vector2f& v)
{
	s << v.x << ", " << v.y;
	return s;
}
std::ostream& operator<<(std::ostream& s, const Vector2i& v)
{
	s << v.x << ", " << v.y;
	return s;
}

template<typename T>
std::ostream& operator<<(std::ostream& s, const glm::mat4& v)
{
	s << v[0][0] << ", " << v[0][1] << ", " << v[0][2] << ", " << v[0][3] << "\n";
	s << v[1][0] << ", " << v[1][1] << ", " << v[1][2] << ", " << v[1][3] << "\n";
	s << v[2][0] << ", " << v[2][1] << ", " << v[2][2] << ", " << v[2][3] << "\n";
	s << v[3][0] << ", " << v[3][1] << ", " << v[3][2] << ", " << v[3][3] << "\n";
	return s;
}

YEASTEM_END
