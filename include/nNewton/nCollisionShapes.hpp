#pragma once

#include <cstdint>

namespace nNewton {

	enum class nCollisionShapeType { nBox, nSphere, nCapsule, nConvex };

	struct nCollisionShape
	{
		nCollisionShapeType type = nCollisionShapeType::nBox;
		uint32_t ColliderID = INVALID_COLLIDER_ID;

		static constexpr uint32_t INVALID_COLLIDER_ID = UINT32_MAX;
	};

	// Pooled shape slot with refcount and generation for handle validation.
	template<class ShapeType>
	struct nCollider {
		uint16_t refCount = 0;
		bool alive = false;
		uint32_t gen = 0;
		ShapeType Collider{};
	};
}
