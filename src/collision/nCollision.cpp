
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
			
			
			ent->currentAABB = GetWorldAABB(ent->EntityShape,ent->EntityTransform,GetColliderPool());
			ent->marginAABB = Expand(ent->currentAABB, FAT_MARGIN);
			m_DynamicTree->UpdateEntity(ent->BVHNodePtr);
		}

		m_DynamicTree->TreeletStepRestructure();

			//read phase
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

	bool nCollisionWorld::UpdateBodyType(
		nEntity_ID id,
		nBodyType newType,
		bool insertNow)
	{
		const bool targetStatic = (newType == nBodyType::Static);

		auto findIn = [&](std::vector<std::unique_ptr<nCollisionEntity>>& container)
			-> std::vector<std::unique_ptr<nCollisionEntity>>::iterator
			{
				return std::find_if(
					container.begin(),
					container.end(),
					[&](const std::unique_ptr<nCollisionEntity>& ent)
					{
						return ent->EntityID == id;
					});
			};

	
		auto& targetContainer = targetStatic ? m_Static_Entities : m_Dynamic_Entities;
		auto targetIt = findIn(targetContainer);
		if (targetIt != targetContainer.end())
			return true;

		auto& sourceContainer = targetStatic ? m_Dynamic_Entities : m_Static_Entities;
		auto sourceIt = findIn(sourceContainer);
		if (sourceIt == sourceContainer.end())
			return false;

		nCollisionEntity* ent = sourceIt->get();
		const bool wasStatic = ent->isStatic;

		if (!wasStatic && ent->BVHNodePtr)
		{
			m_DynamicTree->RemoveEntity(ent->BVHNodePtr);
			ent->BVHNodePtr = nullptr;
		}
		else if (wasStatic)
		{
			ent->BVHNodePtr = nullptr;
		}

		std::unique_ptr<nCollisionEntity> moved = std::move(*sourceIt);
		sourceContainer.erase(sourceIt);


		moved->isStatic = targetStatic;

		nCollisionEntity* ptr = moved.get();
		targetContainer.push_back(std::move(moved));

		if (!targetStatic)
		{
			if (insertNow)
			{
				m_DynamicTree->InsertEntity(ptr);
			}
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