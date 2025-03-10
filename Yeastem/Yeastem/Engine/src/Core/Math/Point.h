#pragma once
//
//#include "Vector2.h"
//#include "Core/OpenGL/RenderAPI/RendererAPI.h"
//
//YEASTEM_BEGIN
//
//class PointMass
//{
//public:
//	friend class Shape;
//	friend std::ostream& operator<<(std::ostream& s, const PointMass& v);
//
//public:
//	PointMass() {}
//	PointMass(float x, float y) :Position(x, y) {}
//	PointMass(Vector2 vec) :Position(vec) {}
//
//public:
//	void SubmitToVertexList(int windowWidth, int windowHeight) const;
//	void SetOffset(Vector2 centre);
//	operator Vector2&() { return Position; }
//	operator const Vector2&() const { return Position; }
//
//public:
//	void Push();
//	void Pop();
//
//public:
//	Vector2 Position;
//
//private:
//	Vector2 m_Offset;
//	std::vector<Vector2> m_Backups;
//
//private:
//	int m_VertexListIndex = -1;
//	std::shared_ptr<Renderable> m_RenderableShape;
//};
//
//std::ostream& operator<<(std::ostream& s, const PointMass& v);
//
//YEASTEM_END
