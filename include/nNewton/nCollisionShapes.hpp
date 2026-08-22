#pragma once

#include <cstdint>

namespace nNewton {
//============
	enum class nCollisionShapeType { nBox, nSphere, nCapsule , nConvex};

	
	struct nCollisionShape
	{
		nCollisionShapeType type;
		uint32_t ColliderID = UINT32_MAX;
	};

	template<class ShapeType>
	struct nCollider {
		uint16_t  refCount = 0;
		bool      alive = false;
		uint32_t gen;
		ShapeType Collider;
	};
}


