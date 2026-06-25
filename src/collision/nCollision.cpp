
#include <nNewton/nCollision.hpp>
#include <list>
#include <iostream>

namespace nNewton
{
	nCollisionWorld::nCollisionWorld()
	{
		m_StaticTree = std::make_unique<nStaticAABBTree<nCollisionEntity>>();
		m_DynamicTree = std::make_unique<nDynamicAABBTree<nCollisionEntity>>();
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
			ent->currentAABB = ent->EntityShape->getAABB(ent->EntityTransform);
			ent->marginAABB = Expand(ent->currentAABB, FAT_MARGIN);
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
			//m_StaticTree->Rebuild(sentity);
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

		if (!isStatic && ent->BVHNodePtr) {
				m_DynamicTree->RemoveEntity(ent->BVHNodePtr); 
				ent->BVHNodePtr = nullptr;
		}
		else
			ent->BVHNodePtr = nullptr;
	
		
		container.erase(it);

		return true;
	}
	void  nCollisionWorld::RemoveAll()
	{
		for (auto& entity : m_Dynamic_Entities)
		{
			if (entity->BVHNodePtr)
			{
				m_DynamicTree->RemoveEntity(entity->BVHNodePtr);
				entity->BVHNodePtr = nullptr;
			}
		}
		for (auto& entity : m_Static_Entities){
			entity->BVHNodePtr = nullptr;
		}
		m_Dynamic_Entities.clear();
		m_Static_Entities.clear();
		m_DynamicTree->Clear();
		m_StaticTree->Clear();
	}
	bool nCollisionWorld::RebuildBVH(bool isStatic)
	{
		if (isStatic)
		{
			if (m_Static_Entities.empty() || !m_StaticTree)
				return false;

			auto entities = ToRawPtrs(m_Static_Entities);
			m_StaticTree->Rebuild(entities);
		}
		else
		{
			if (m_Dynamic_Entities.empty() || !m_DynamicTree)
				return false;

			auto entities = ToRawPtrs(m_Dynamic_Entities);
			m_DynamicTree->Rebuild(entities);
		}
		return true;
	}

	void nCollisionWorld::QueryAllOverlappingPairs(std::vector<std::pair<nCollisionEntity*, nCollisionEntity*>>& OverlapEntities)
	{
		if (m_StaticTree->GetRoot())
			nAABBTree<nCollisionEntity>::TraverseOverlaps(OverlapEntities, m_StaticTree->GetRoot(), m_StaticTree->GetRoot());
		if (m_DynamicTree->GetRoot())
			nAABBTree<nCollisionEntity>::TraverseOverlaps(OverlapEntities, m_DynamicTree->GetRoot(), m_DynamicTree->GetRoot());
		if (m_DynamicTree->GetRoot() && m_StaticTree->GetRoot())
			nAABBTree<nCollisionEntity>::TraverseCrossOverlaps(OverlapEntities,m_DynamicTree->GetRoot(), m_StaticTree->GetRoot());
	}

	void nCollisionWorld::QueryOverlap(std::vector<std::pair<nCollisionEntity*, nCollisionEntity*>>& OverlapEntities, const nCollisionEntity* Entity)
	{
		if (!Entity->BVHNodePtr)return;

		if (Entity->isStatic)
		{
			if (m_StaticTree->GetRoot())
				nAABBTree<nCollisionEntity>::TraverseOverlaps(OverlapEntities, Entity->BVHNodePtr, m_StaticTree->GetRoot());
			if (m_DynamicTree->GetRoot())
				nAABBTree<nCollisionEntity>::TraverseCrossOverlaps(OverlapEntities, Entity->BVHNodePtr, m_DynamicTree->GetRoot());
		}
		else
		{
			if (m_StaticTree->GetRoot())
				nAABBTree<nCollisionEntity>::TraverseCrossOverlaps(OverlapEntities, Entity->BVHNodePtr, m_StaticTree->GetRoot());
			if (m_DynamicTree->GetRoot())
				nAABBTree<nCollisionEntity>::TraverseOverlaps(OverlapEntities, Entity->BVHNodePtr, m_DynamicTree->GetRoot());
		}
	}


} //namespace nNewton