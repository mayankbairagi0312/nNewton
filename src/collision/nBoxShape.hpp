#include "nCollisionShapes.hpp"

namespace nNewton
{
	class nBoxShape : public nCollisionShape
	{

	public:
		nVector3 m_HalfExtents;
		nBoxShape(const nVector3& halfExtents_) : m_HalfExtents(halfExtents_)
		{ }
		virtual nAABB getAABB(const nTransform& transform_) const override
		{
			nAABB aabb;
			nVector3 center = transform_.GetPosition();
			nVector3 e = m_HalfExtents * transform_.GetScale();
			nVector3 corn[8] = {
			{ -e.x, -e.y, -e.z },
			{  e.x, -e.y, -e.z },
			{ -e.x,  e.y, -e.z },
			{  e.x,  e.y, -e.z },
			{ -e.x, -e.y,  e.z },
			{  e.x, -e.y,  e.z },
			{ -e.x,  e.y,  e.z },
			{  e.x,  e.y,  e.z }
				};

			aabb.min = aabb.max = center + transform_.TransformVec(corn[0]);
			for (size_t i = 0; i < 8; i++)
			{
				nVector3 worldCorner = center + transform_.TransformVec(corn[i]);
				aabb.min = Min(aabb.min, worldCorner);
				aabb.max = Max(aabb.max, worldCorner);
			}
			
			return aabb;
		}
		virtual nVector3 getSupportPoint(const nVector3& direction) const override
		{
			nVector3 supportPoint;
			supportPoint.x = (direction.x >= 0) ? m_HalfExtents.x : -m_HalfExtents.x;
			supportPoint.y = (direction.y >= 0) ? m_HalfExtents.y : -m_HalfExtents.y;
			supportPoint.z = (direction.z >= 0) ? m_HalfExtents.z : -m_HalfExtents.z;
			return supportPoint;
		}
	};
}