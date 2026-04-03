

#include <nNewton/nCollision.hpp>
#include "Collision_Broadphase/nDynamicAABBTree.cpp"
#include "Collision_Broadphase/nStaticAABBTree.cpp"

namespace nNewton
{
	nCollisionWorld::nCollisionWorld()
	{
		m_StaticTree = std::make_unique<nStaticAABBTree>();
		m_DynamicTree = std::make_unique<nDynamicAABBTree>();
	}
	bool nCollisionWorld::INIT_COLLISION_WORLD()
	{
		BuildTrees();
		return true;
	}
	void nCollisionWorld::BuildTrees()
	{
		auto rawPtrVec = ToRawPtrs(m_Static_Entities);
		m_StaticTree->BuildAABBTree(rawPtrVec);
		
	}

	//void nCollisionWorld::UpdateCollisionEntity(nCollisionEntity* entity)
	//{
	//	//m_DynamicTree->UpdateEntity(entity->BVHNodePtr);

	//}

	nCollisionEntity* nCollisionWorld::CreateCollisionEntity(nEntity_ID& ID, bool isStatic, const nTransform& EntityTransform, const nVector3& vel)
	{

		nCollisionEntity data;
		data.EntityID = ID;
		data.isStatic = isStatic;
		data.EntityTransform = EntityTransform;
		data.vel = vel;

		auto ent = std::make_unique<nCollisionEntity>(std::move(data));
		nCollisionEntity* ptr = ent.get();

		if (ent->isStatic)
		{
			m_Static_Entities.push_back(std::move(ent));
		}
		else
		{
			m_Dynamic_Entities.push_back(std::move(ent));
		}

		return ptr;

	}
}