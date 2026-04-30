#pragma once


#include"nCollisionShapes.hpp"
#include "nTransform.hpp"
#include "nTypes.hpp"
#include <memory>

namespace nNewton
{
	struct nBVHNode;   
	struct nCollisionEntity
	{
		nBVHNode* BVHNodePtr;
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