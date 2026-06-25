#pragma once

#include"nBVHNode.hpp"
#include"nCollisionShapes.hpp"
#include "nTransform.hpp"
#include "nTypes.hpp"
#include <memory>

namespace nNewton
{
	constexpr float FAT_MARGIN = 0.01f;
	struct nCollisionEntity
	{
		nBVHNode<nCollisionEntity>* BVHNodePtr;
		std::shared_ptr<nCollisionShape> EntityShape;
		nTransform EntityTransform;
		nAABB marginAABB;
		nAABB currentAABB;
		nVector3 vel;
		nEntity_ID EntityID;
		bool isStatic;
		bool isSleeping;
	};
}