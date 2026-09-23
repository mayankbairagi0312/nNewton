#pragma once

#include "nBVHNode.hpp"
#include "nCollisionShapes.hpp"
#include "nTransform.hpp"
#include "nTypes.hpp"
#include <memory>

namespace nNewton
{
	constexpr float FAT_MARGIN = 0.01f;

	struct nCollisionEntity
	{
		// -- Constructors --
		nCollisionEntity() = default;
		nCollisionEntity(const nCollisionEntity&) = delete;
		nCollisionEntity& operator=(const nCollisionEntity&) = delete;
		nCollisionEntity(nCollisionEntity&&) noexcept = default;
		nCollisionEntity& operator=(nCollisionEntity&&) & noexcept = default;
		~nCollisionEntity() = default;

		nBVHNode<nCollisionEntity>* BVHNodePtr = nullptr;
		nCollisionShape EntityShape{};
		nTransform EntityTransform;
		nAABB marginAABB;
		nAABB currentAABB;
		nVector3 vel;
		nEntity_ID EntityID = INVALID_ENTITY;
		bool isStatic = false;
		bool isSleeping = false;
	};
}
