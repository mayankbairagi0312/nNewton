#pragma once

#include "nMath.hpp"
#include "nCollisionShapes.hpp"
#include "nTransform.hpp"
#include "nAABB.hpp"

namespace nNewton
{
	class nSphereShape
	{
	public:
		// -- Constructors --
		nSphereShape() = default;
		explicit nSphereShape(float radius_) noexcept
			: radius(radius_)
		{
		}
		nSphereShape(const nSphereShape&) = default;
		nSphereShape(nSphereShape&&) noexcept = default;
		nSphereShape& operator=(const nSphereShape&) = default;
		nSphereShape& operator=(nSphereShape&&) noexcept = default;
		~nSphereShape() = default;

		static constexpr nCollisionShapeType nTYPE = nCollisionShapeType::nSphere;

		nAABB getAABB(const nTransform& transform) const noexcept
		{
			const nVector3 worldCenter = transform.GetPosition();
			const float worldRadius = radius * transform.GetScale().x;
			const nVector3 extents(worldRadius, worldRadius, worldRadius);

			return nAABB(worldCenter - extents, worldCenter + extents);
		}

		nVector3 getSupportPoint(const nVector3& direction) const noexcept
		{
			const float len = direction.Length();
			if (len < EPSILON)
				return nVector3(0.0f, 0.0f, 0.0f);

			const nVector3 normDir = direction / len;
			return normDir * radius;
		}

		nVector3 getCentroid() const noexcept { return nVector3(0.0f, 0.0f, 0.0f); }

		float getVolume() const noexcept
		{
			return (4.0f / 3.0f) * PI * radius * radius * radius;
		}

		// Unit inertia tensor (I/mass) — all diagonal elements equal: (2/5)·r²
		nMatrix3 getUnitInertia() const noexcept
		{
			const float I = (2.0f / 5.0f) * radius * radius;
			return nMatrix3{
				I, 0.0f, 0.0f,
				0.0f, I, 0.0f,
				0.0f, 0.0f, I
			};
		}

		float radius = 1.0f;
	};
}
