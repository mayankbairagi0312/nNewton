#include "nMath.hpp"
#include "nTransform.hpp"


namespace nNewton
{

	struct nAABB
	{
		nVector3 min;
		nVector3 max;
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

	class nCollisionShape
	{
	public:

		virtual nAABB getAABB(const nTransform& transform_)const = 0;
		virtual nVector3 getSupportPoint(const nVector3& direction)const = 0;

		virtual ~nCollisionShape() = default;
		
	};
}


