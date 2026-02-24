#include "nCollisionShapes.hpp"

namespace nNewton
{
	class nSphereShape : public nCollisionShape
	{
		float radius;
		nSphereShape(const float radi) : radius(radi)
		{ }

		nAABB getAABB(const nTransform& transform_)const override
		{
			auto WorldCenter = transform_.GetPosition();
			auto scale = transform_.GetScale();

			auto e = nVector3(radius*scale.x, radius*scale.y, radius*scale.z);

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

			nAABB aabb;
			aabb.min = aabb.max = WorldCenter + transform_.TransformVec(corn[0]);
			for (size_t i = 1; i < 8; i++)
			{
				nVector3 worldCorner = WorldCenter + transform_.TransformVec(corn[i]);
				aabb.min = Min(aabb.min, worldCorner);
				aabb.max = Max(aabb.max, worldCorner);
			}

			return aabb;

			//return nAABB(WorldCenter - extents, WorldCenter + extents);
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