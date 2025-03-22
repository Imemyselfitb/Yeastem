#pragma once

YEASTEM_BEGIN

class Vector2
{
public:
	friend std::ostream& operator<<(std::ostream& s, const Vector2& v);

public:
	Vector2() {}
	Vector2(float x, float y) :x(x), y(y) {}
	Vector2(const Vector2& other) :x(other.x), y(other.y) {}
	~Vector2() {}

public:
	void operator=(const Vector2& other) { x = other.x; y = other.y; }
	void set(Vector2 other);
	void set(float x, float y);
	Vector2 copy() { return Vector2{ x, y }; }

	Vector2 operator+(Vector2 other) const;
	Vector2 operator-(Vector2 other) const;
	Vector2 operator*(Vector2 other) const;
	Vector2 operator/(Vector2 other) const;
	Vector2 operator*(float scalar) const;
	Vector2 operator/(float scalar) const;

	void operator+=(Vector2 other);
	void operator-=(Vector2 other);
	void operator*=(Vector2 other);
	void operator/=(Vector2 other);
	void operator*=(float scalar);
	void operator/=(float scalar);

	void Rotate(float angle, Vector2 pivot = { 0, 0 });
	void Scale(float sf, Vector2 centre = { 0, 0 });

	Vector2 RotateNew(float angle, Vector2 pivot = { 0, 0 }) { Vector2 c = copy(); c.Rotate(angle, pivot); return c; }
	Vector2 ScaleNew(float sf, Vector2 centre = { 0, 0 }) { Vector2 c = copy(); c.Scale(sf, centre); return c; }

public:
	float x = 0, y = 0;
};

YEASTEM_END
