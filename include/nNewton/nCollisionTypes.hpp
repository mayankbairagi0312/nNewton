#pragma once

#include"nCollisionShapes.hpp"
#include "nTransform.hpp"
#include "nDynamicsWorld.hpp"

namespace nNewton
{
	struct nCollisionEntity
	{
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