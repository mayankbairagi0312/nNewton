#pragma once

#include "nMath.hpp"
#include "nCollisionShapes.hpp"
#include "nTransform.hpp"
#include "nAABB.hpp"

namespace nNewton
{
	class nBoxShape 
	{

	public:
		nVector3 m_HalfExtents;
		nBoxShape() = default;
		explicit nBoxShape(nVector3 halfExtents) : m_HalfExtents(halfExtents) {}

		static const nCollisionShapeType nTYPE = nCollisionShapeType::nBox;
		nAABB getAABB(const nTransform& transform_) const 
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
			for (size_t i = 1; i < 8; i++)
			{
				nVector3 worldCorner = center + transform_.TransformVec(corn[i]);
				aabb.min = Min(aabb.min, worldCorner);
				aabb.max = Max(aabb.max, worldCorner);
			}
			
			//printf("HalfExtents: %.2f %.2f %.2f\n", m_HalfExtents.x, m_HalfExtents.y, m_HalfExtents.z);
			//printf("Scale: %.2f %.2f %.2f\n", transform_.GetScale().x, transform_.GetScale().y, transform_.GetScale().z);
			//printf("AABB size: %.2f %.2f %.2f\n", aabb.max.x - aabb.min.x, aabb.max.y - aabb.min.y, aabb.max.z - aabb.min.z);

			return aabb;
		}

		nVector3 getSupportPoint(const nVector3& direction) const 
		{
			nVector3 supportPoint;
			//GJK/collision code transforms the support point to world space afterward and applies scale 
			supportPoint.x = (direction.x >= 0) ? m_HalfExtents.x : -m_HalfExtents.x;
			supportPoint.y = (direction.y >= 0) ? m_HalfExtents.y : -m_HalfExtents.y;
			supportPoint.z = (direction.z >= 0) ? m_HalfExtents.z : -m_HalfExtents.z;
			return supportPoint;
		}
	
		nVector3 getCentroid() const { return nVector3(0.0f, 0.0f, 0.0f);}

		float getVolume() const { return 8.0f * m_HalfExtents.x * m_HalfExtents.y * m_HalfExtents.z;}

		// Inertia tensor per unit mass (diagonal) for a box:
		// Ixx/m = (hy² + hz²)/3,  Iyy/m = (hx² + hz²)/3,  Izz/m = (hx² + hy²)/3
		nMatrix3 getUnitInertia() const 
		{
			float hx = m_HalfExtents.x;
			float hy = m_HalfExtents.y;
			float hz = m_HalfExtents.z;
			float Ixx = (hy * hy + hz * hz) / 3.0f;
			float Iyy = (hx * hx + hz * hz) / 3.0f;
			float Izz = (hx * hx + hy * hy) / 3.0f;
			return nMatrix3{
				Ixx, 0.0f, 0.0f,   
				0.0f, Iyy, 0.0f,  
				0.0f, 0.0f, Izz   
			};
		}
	};
}