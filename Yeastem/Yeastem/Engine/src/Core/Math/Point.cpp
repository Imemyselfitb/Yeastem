#include "yst_pch.h"
//
#include "Point.h"
//
//YEASTEM_BEGIN
//
//void PointMass::Push()
//{
//	m_Backups.push_back(Vector2(Position));
//}
//
//void PointMass::Pop()
//{
//	Vector2 last = m_Backups[m_Backups.size() - 1];
//	Position = last;
//	m_Backups.pop_back();
//}
//
//void PointMass::SubmitToVertexList(int windowWidth, int windowHeight) const
//{
//	Vector2 onScreenPos = Position / Vector2(windowWidth / 2.0f, windowHeight / 2.0f);
//	onScreenPos -= Vector2(1.0, 1.0);
//	m_RenderableShape->VertexList[m_VertexListIndex].position.set(onScreenPos);
//}
//
//void PointMass::SetOffset(Vector2 centre)
//{
//	m_Offset = Position - centre;
//}
//
//std::ostream& operator<<(std::ostream& s, const PointMass& pt)
//{
//	s << "Position: " << pt.Position << " Offset: " << pt.m_Offset;
//	return s;
//}
//
//YEASTEM_END

