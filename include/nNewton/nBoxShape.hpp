#pragma once

#include "nMath.hpp"
#include "nCollisionShapes.hpp"
#include "nTransform.hpp"
#include "nAABB.hpp"

namespace nNewton
{
	// Box collision shape defined by half extents in local space.
	class nBoxShape
	{
	public:
		// -- Constructors --
		nBoxShape() = default;
		explicit nBoxShape(nVector3 halfExtents) noexcept
			: m_HalfExtents(halfExtents)
		{
		}
		nBoxShape(const nBoxShape&) = default;
		nBoxShape(nBoxShape&&) noexcept = default;
		nBoxShape& operator=(const nBoxShape&) = default;
		nBoxShape& operator=(nBoxShape&&) noexcept = default;
		~nBoxShape() = default;

		static constexpr nCollisionShapeType nTYPE = nCollisionShapeType::nBox;

		// World-space AABB of the scaled, rotated box.
		nAABB getAABB(const nTransform& transform) const
		{
			nAABB aabb;
			const nVector3 center = transform.GetPosition();

			const nVector3 e = m_HalfExtents * transform.GetScale();
			const nVector3 corners[8] = {
				{ -e.x, -e.y, -e.z },
				{  e.x, -e.y, -e.z },
				{ -e.x,  e.y, -e.z },
				{  e.x,  e.y, -e.z },
				{ -e.x, -e.y,  e.z },
				{  e.x, -e.y,  e.z },
				{ -e.x,  e.y,  e.z },
				{  e.x,  e.y,  e.z }
			};

			aabb.min = aabb.max = center + transform.TransformVec(corners[0]);
			for (size_t i = 1; i < 8; ++i)
			{
				const nVector3 worldCorner = center + transform.TransformVec(corners[i]);
				aabb.min = Min(aabb.min, worldCorner);
				aabb.max = Max(aabb.max, worldCorner);
			}

			return aabb;
		}

		// Farthest point on the shape in a given direction (local space).
		// GJK/collision code transforms the support point to world space afterward and applies scale.
		nVector3 getSupportPoint(const nVector3& direction) const noexcept
		{
			return nVector3(
				(direction.x >= 0.0f) ? m_HalfExtents.x : -m_HalfExtents.x,
				(direction.y >= 0.0f) ? m_HalfExtents.y : -m_HalfExtents.y,
				(direction.z >= 0.0f) ? m_HalfExtents.z : -m_HalfExtents.z
			);
		}

		nVector3 getCentroid() const noexcept { return nVector3(0.0f, 0.0f, 0.0f); }

		float getVolume() const noexcept
		{
			return 8.0f * m_HalfExtents.x * m_HalfExtents.y * m_HalfExtents.z;
		}

		// Inertia tensor per unit mass (diagonal) for a box:
		// Ixx/m = (hy² + hz²)/3,  Iyy/m = (hx² + hz²)/3,  Izz/m = (hx² + hy²)/3
		nMatrix3 getUnitInertia() const noexcept
		{
			const float hx = m_HalfExtents.x;
			const float hy = m_HalfExtents.y;
			const float hz = m_HalfExtents.z;
			const float Ixx = (hy * hy + hz * hz) / 3.0f;
			const float Iyy = (hx * hx + hz * hz) / 3.0f;
			const float Izz = (hx * hx + hy * hy) / 3.0f;
			return nMatrix3{
				Ixx, 0.0f, 0.0f,
				0.0f, Iyy, 0.0f,
				0.0f, 0.0f, Izz
			};
		}

		nVector3 m_HalfExtents{ 1.0f, 1.0f, 1.0f };
	};
}
