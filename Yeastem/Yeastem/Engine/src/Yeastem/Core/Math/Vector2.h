#pragma once

YEASTEM_BEGIN

template<typename T>
class Vector2
{
public:
	Vector2() {}
	Vector2(T x, T y) :x(x), y(y) {}
	Vector2(const Vector2<T>& other) :x(other.x), y(other.y) {}
	~Vector2() {}

	template<typename T2> Vector2(const Vector2<T2>& other) : x((T)other.x), y((T)other.y) {}
	template<typename T2> Vector2(T2 x, T2 y) :x((T)x), y((T)y) {}

public:
	void operator=(const Vector2<T>& other) { x = other.x; y = other.y; }
	Vector2<T> copy() const { return Vector2<T>{ x, y }; }
	template<typename T2> Vector2<T> copy() const { return Vector2<T2>{ x, y }; }
	void set(Vector2<T> other);
	void set(T x, T y);

	Vector2<T> operator+(Vector2<T> other) const;
	Vector2<T> operator-(Vector2<T> other) const;
	Vector2<T> operator*(Vector2<T> other) const;
	Vector2<T> operator/(Vector2<T> other) const;
	Vector2<T> operator*(T scalar) const;
	Vector2<T> operator/(T scalar) const;

	void operator+=(Vector2<T> other);
	void operator-=(Vector2<T> other);
	void operator*=(Vector2<T> other);
	void operator/=(Vector2<T> other);
	void operator*=(T scalar);
	void operator/=(T scalar);

	void Rotate(float angle, Vector2<T> pivot = { 0, 0 });
	void Scale(T sf, Vector2<T> centre = { 0, 0 });

	Vector2<T> RotateNew(float angle, Vector2<T> pivot = { 0, 0 }) const 
	{ Vector2<T> c = copy(); c.Rotate(angle, pivot); return c; }
	Vector2<T> ScaleNew(T sf, Vector2<T> centre = { 0, 0 }) const 
	{ Vector2<T> c = copy(); c.Scale(sf, centre); return c; }

public:
	friend std::ostream& operator<<(std::ostream & s, const Vector2<T>&v)
	{
		s << v.x << ", " << v.y;
		return s;
	}

public:
	T x = 0, y = 0;
};

// Expicit Template Instantiation
#ifndef YEASTEM_VECTOR2_CPP
#define TEMPLATE_INSTANCE extern template class
#else
#define TEMPLATE_INSTANCE template class
#endif

TEMPLATE_INSTANCE Vector2<float>;
using Vector2f = Vector2<float>;
TEMPLATE_INSTANCE Vector2<int>;
using Vector2i = Vector2<int>;
#undef TEMPLATE_INSTANCE

YEASTEM_END
