#include "yst_pch.h"

#define YEASTEM_VECTOR2_CPP

#include "Vector2.h"

YEASTEM_BEGIN

template class Vector2<float>;
template class Vector2<int>;

template<typename T>
void Vector2<T>::set(Vector2 other)
{
	x = other.x;
	y = other.y;
}

template<typename T>
void Vector2<T>::set(T xNew, T yNew)
{
	x = xNew;
	y = yNew;
}

template<typename T>
Vector2<T> Vector2<T>::operator+(Vector2 other) const
{
	return { x + other.x, y + other.y };
}
template<typename T>
Vector2<T> Vector2<T>::operator-(Vector2 other) const
{
	return { x - other.x, y - other.y };
}
template<typename T>
Vector2<T> Vector2<T>::operator*(Vector2 other) const
{
	return { x * other.x, y * other.y };
}
template<typename T>
Vector2<T> Vector2<T>::operator/(Vector2 other) const
{
	return { x / other.x, y / other.y };
}
template<typename T>
Vector2<T> Vector2<T>::operator*(T scalar) const
{
	return { x * scalar, y * scalar };
}
template<typename T>
Vector2<T> Vector2<T>::operator/(T scalar) const
{
	return { x / scalar, y / scalar };
}

template<typename T>
void Vector2<T>::operator+=(Vector2 other)
{
	x += other.x;
	y += other.y;
}
template<typename T>
void Vector2<T>::operator-=(Vector2 other)
{
	x -= other.x;
	y -= other.y;
}
template<typename T>
void Vector2<T>::operator*=(Vector2 other)
{
	x *= other.x;
	y *= other.y;
}
template<typename T>
void Vector2<T>::operator/=(Vector2 other)
{
	x /= other.x;
	y /= other.y;
}
template<typename T>
void Vector2<T>::operator*=(T scalar)
{
	x *= scalar;
	y *= scalar;
}
template<typename T>
void Vector2<T>::operator/=(T scalar)
{
	x /= scalar;
	y /= scalar;
}

template<typename T>
void Vector2<T>::Rotate(float Angle, Vector2 Pivot)
{
	T s = (T)sin(Angle);
	T c = (T)cos(Angle);

	T xOff = x - Pivot.x;
	T yOff = y - Pivot.y;

	x = (xOff * c) - (yOff * s) + Pivot.x;
	y = (xOff * s) + (yOff * c) + Pivot.y;
}

template<typename T>
void Vector2<T>::Scale(T sf, Vector2 centre)
{
	T xOff = x - centre.x;
	T yOff = y - centre.y;

	x = sf * xOff + centre.x;
	y = sf * yOff + centre.y;
}

YEASTEM_END

