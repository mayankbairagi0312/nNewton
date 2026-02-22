#include <nNewton/nMath.hpp>
#include <nNewton/nTransform.hpp>


namespace nNewton
{

	struct nAABB
	{
		nVector3 min;
		nVector3 max;
	};

	class nCollisionShape
	{
	public:

		virtual nAABB getAABB(const nTransform& transform_)const = 0;
		virtual nVector3 getSupportPoint(const nVector3& direction)const = 0;

		virtual ~nCollisionShape() = default;
		
	};
}


