#pragma once

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

YEASTEM_BEGIN

using Vector2i = glm::ivec2;

class Vector2f : public glm::vec2 {
public:
	Vector2f() : glm::vec2() {}
	Vector2f(float x, float y) : glm::vec2(x, y) {}
	Vector2f(const glm::vec2& value) : glm::vec2(value) {}
	Vector2f(Vector2i iValue) : glm::vec2(iValue.x, iValue.y) {}

public:
	Vector2f Rotated(float angle) const;
};

// Output stream operators for easy printing of vectors and matrices
std::ostream& operator<<(std::ostream& s, const Vector2f& v);
std::ostream& operator<<(std::ostream& s, const Vector2i& v);
std::ostream& operator<<(std::ostream& s, const glm::mat4& v);

YEASTEM_END
