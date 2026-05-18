
#include <nNewton/nCollision.hpp>
#include "Collision_Broadphase/nDynamicAABBTree.cpp"
#include "Collision_Broadphase/nStaticAABBTree.cpp"
#include <list>

namespace nNewton
{
	
	nCollisionWorld::nCollisionWorld()
	{
		m_StaticTree = std::make_unique<nStaticAABBTree>();
		m_DynamicTree = std::make_unique<nDynamicAABBTree>();
	}
	bool nCollisionWorld::INIT_COLLISION_WORLD()
	{
		return true;
	}
	void nCollisionWorld::BuildTrees()
	{
		auto rawPtrVec = ToRawPtrs(m_Static_Entities);
		if(!rawPtrVec.empty())m_StaticTree->BuildAABBTree(rawPtrVec);
		rawPtrVec.clear();
		rawPtrVec = ToRawPtrs(m_Dynamic_Entities);
		if (!rawPtrVec.empty())m_DynamicTree->BuildAABBTree(rawPtrVec);

	}

	void nCollisionWorld::StepCollision()
	{
		//m_DynamicTree->UpdateEntity(entity->BVHNodePtr);
		
			// write phase
		auto dentity = ToRawPtrs(m_Dynamic_Entities);
		for (auto& ent : dentity) {
			m_DynamicTree->UpdateEntity(ent->BVHNodePtr);
		}

		m_DynamicTree->TreeletStepRestructure(); 

			//read phase
	}

	nCollisionEntity* nCollisionWorld::CreateCollisionEntity(nEntity_ID& ID, bool isStatic, const nTransform& EntityTransform, const nVector3& vel,
		std::shared_ptr<nCollisionShape> ColisionShape, bool insertNow = true)
	{

		nCollisionEntity data;
		data.EntityID = ID;
		data.isStatic = isStatic;
		data.EntityTransform = EntityTransform;
		data.vel = vel;
		data.EntityShape = ColisionShape;

		data.currentAABB = ColisionShape->getAABB(EntityTransform);
		data.marginAABB = Expand(data.currentAABB, FAT_MARGIN);
	

		auto ent = std::make_unique<nCollisionEntity>(std::move(data));
		nCollisionEntity* ptr = ent.get();

		if (ent->isStatic)
		{
			m_Static_Entities.push_back(std::move(ent));
			
			auto sentity = ToRawPtrs(m_Static_Entities); 
			m_StaticTree->Rebuild(sentity);
				
		}
		else
		{
			m_Dynamic_Entities.push_back(std::move(ent));
			if (insertNow)
				m_DynamicTree->InsertEntity(ptr);
		}
		
		return ptr;

	}


	//later replace with swap and pop & id map somthing ...  
	bool nCollisionWorld::RemoveCollisionEntity(nEntity_ID& ID, bool isStatic)
	{
		auto& container = isStatic ? m_Static_Entities : m_Dynamic_Entities;

		
		auto it = std::find_if(container.begin(), container.end(),
			[&ID](const std::unique_ptr<nCollisionEntity>& ent) {
				return ent->EntityID == ID;
			});

		if (it == container.end()) return false; 

		nCollisionEntity* ent = it->get();

		if (!isStatic) {
			if (ent->BVHNodePtr)
				m_DynamicTree->RemoveEntity(ent->BVHNodePtr); 
		}

		container.erase(it);

		if (isStatic) {
			auto sentity = ToRawPtrs(m_Static_Entities);
			m_StaticTree->Rebuild(sentity);
		}

		return true;
	}
	

	void nCollisionWorld::QueryAllOverlappingPairs(std::vector<std::pair<nCollisionEntity*, nCollisionEntity*>>& OverlapEntities)
	{
		if (m_StaticTree->GetRoot())
			nAABBTree::TraverseOverlaps(OverlapEntities, m_StaticTree->GetRoot(), m_StaticTree->GetRoot());
		if (m_DynamicTree->GetRoot())
			nAABBTree::TraverseOverlaps(OverlapEntities, m_DynamicTree->GetRoot(), m_DynamicTree->GetRoot());
		if (m_DynamicTree->GetRoot() && m_StaticTree->GetRoot())
			nAABBTree::TraverseCrossOverlaps(OverlapEntities,m_DynamicTree->GetRoot(), m_StaticTree->GetRoot());
	}

} //namespace nNewton