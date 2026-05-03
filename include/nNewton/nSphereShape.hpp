#pragma once

#include "nCollisionShapes.hpp"

namespace nNewton
{
	class nSphereShape : public nCollisionShape
	{
	public:
		float radius;
		nSphereShape(const float radi) : radius(radi)
		{ }

		nAABB getAABB(const nTransform& transform_)const override
		{
			auto WCenter = transform_.GetPosition();
			float WRadius = radius * transform_.GetScale().x;
			nVector3 ext(WRadius, WRadius, WRadius);

			return nAABB(WCenter - ext , WCenter + ext);

		}

		// farthest point on the shape in a given direction (local space).
		nVector3 getSupportPoint(const nVector3& direction)const override
		{
			auto len = direction.Length();
			if (len  < EPSILON) return nVector3(0, 0, 0);
			
			auto normDir = direction / len;
			return normDir * radius;
		}
	};
}