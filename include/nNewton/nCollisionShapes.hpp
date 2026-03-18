#pragma once
#include "nMath.hpp"
#include "nTransform.hpp"


namespace nNewton
{

	struct nAABB
	{
		nVector3 min;
		nVector3 max;

		nVector3 Center()      const { return (min + max) * 0.5f; }
		nVector3 HalfExtents() const { return (max - min) * 0.5f; }
		float SurfaceArea() const
		{
			nVector3 e = max - min;  // extents
			return 2.0f * (e.x * e.y + e.y * e.z + e.z * e.x);
		}
	};

	nAABB Expand(const nAABB& a, const float marg_)
	{
		nAABB eaabb;
		eaabb.min = nVector3(a.min.x - marg_, a.min.y - marg_, a.min.z - marg_);
		eaabb.max = nVector3( a.max.x + marg_,a.max.y + marg_,a.max.z + marg_ );
		return eaabb;
	}
	nAABB Merge(const nAABB& a, const nAABB& b)
	{
		nAABB maabb;
		maabb.max = Max(a.max, b.max);
		maabb.min = Min(a.min, b.min);
		return maabb;
	}
	bool Contains(const nAABB& this_aabb, const nAABB& that_aabb) {
		return this_aabb.min.x <= that_aabb.min.x &&
			this_aabb.min.y <= that_aabb.min.y &&
			this_aabb.min.z <= that_aabb.min.z &&
			this_aabb.max.x >= that_aabb.max.x &&
			this_aabb.max.y >= that_aabb.max.y &&
			this_aabb.max.z >= that_aabb.max.z;
	}

	bool Overlaps(const nAABB& n , const nAABB& m) 
	{
		return (n.min.x <= m.max.x && n.max.x >= m.min.x) &&
			(n.min.y <= m.max.y && n.max.y >= m.min.y) &&
			(n.min.z <= m.max.z && n.max.z >= m.min.z);
	}
	

	
//============

	class nCollisionShape
	{
	public:

		virtual nAABB getAABB(const nTransform& transform_)const = 0;
		virtual nVector3 getSupportPoint(const nVector3& direction)const = 0;

		virtual ~nCollisionShape() = default;
		
	};
}


