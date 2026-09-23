#include <nNewton/nCollision.hpp>

namespace nNewton
{
	nCollisionWorld::nCollisionWorld()
	{
		m_StaticTree = std::make_unique<nStaticAABBTree<nCollisionEntity>>();
		m_DynamicTree = std::make_unique<nDynamicAABBTree<nCollisionEntity>>();
	}

	void nCollisionWorld::BuildTrees()
	{
		// Static tree: full rebuild over all static proxies.
		std::vector<nCollisionEntity*> rawPtrVec = ToRawPtrs(m_StaticEntities);
		if (!rawPtrVec.empty()) m_StaticTree->BuildAABBTree(rawPtrVec);

		rawPtrVec = ToRawPtrs(m_DynamicEntities);
		if (!rawPtrVec.empty()) m_DynamicTree->BuildAABBTree(rawPtrVec);
	}

	void nCollisionWorld::StepCollision()
	{
		// Write phase: refresh AABBs and update the BVH.
		std::vector<nCollisionEntity*> dynamicEntities = ToRawPtrs(m_DynamicEntities);
		for (nCollisionEntity* entity : dynamicEntities) {
			entity->currentAABB = GetWorldAABB(entity->EntityShape, entity->EntityTransform, GetColliderPool());
			entity->marginAABB = Expand(entity->currentAABB, FAT_MARGIN);
			m_DynamicTree->UpdateEntity(entity->BVHNodePtr);
		}

		// Restructure phase: process the refit queue.
		m_DynamicTree->TreeletStepRestructure();

		// Read phase.
	}

	void nCollisionWorld::RemoveAll()
	{
		for (auto& entity : m_DynamicEntities)
		{
			if (entity->BVHNodePtr)
			{
				m_DynamicTree->RemoveEntity(entity->BVHNodePtr);
				entity->BVHNodePtr = nullptr;
			}
		}
		for (auto& entity : m_StaticEntities) {
			entity->BVHNodePtr = nullptr;
		}
		m_DynamicEntities.clear();
		m_StaticEntities.clear();
		m_DynamicTree->Clear();
		m_StaticTree->Clear();
	}

	bool nCollisionWorld::RebuildBVH(bool isStatic)
	{
		if (isStatic)
		{
			if (m_StaticEntities.empty() || !m_StaticTree)
				return false;

			std::vector<nCollisionEntity*> entities = ToRawPtrs(m_StaticEntities);
			m_StaticTree->Rebuild(entities);
		}
		else
		{
			if (m_DynamicEntities.empty() || !m_DynamicTree)
				return false;

			std::vector<nCollisionEntity*> entities = ToRawPtrs(m_DynamicEntities);
			m_DynamicTree->Rebuild(entities);
		}
		return true;
	}

	bool nCollisionWorld::UpdateBodyType(nEntity_ID id, nBodyType newType, bool insertNow)
	{
		const bool targetStatic = (newType == nBodyType::Static);

		auto findIn = [id](std::vector<std::unique_ptr<nCollisionEntity>>& container)
			-> std::vector<std::unique_ptr<nCollisionEntity>>::iterator
		{
			return std::find_if(
				container.begin(),
				container.end(),
				[id](const std::unique_ptr<nCollisionEntity>& entity)
				{
					return entity->EntityID == id;
				});
		};

		// Already in the target container: nothing to do.
		std::vector<std::unique_ptr<nCollisionEntity>>& targetContainer = targetStatic ? m_StaticEntities : m_DynamicEntities;
		auto targetIt = findIn(targetContainer);
		if (targetIt != targetContainer.end())
			return true;

		std::vector<std::unique_ptr<nCollisionEntity>>& sourceContainer = targetStatic ? m_DynamicEntities : m_StaticEntities;
		auto sourceIt = findIn(sourceContainer);
		if (sourceIt == sourceContainer.end())
			return false;

		nCollisionEntity* entity = sourceIt->get();
		const bool wasStatic = entity->isStatic;

		// Detach from the source tree first.
		if (!wasStatic && entity->BVHNodePtr)
		{
			m_DynamicTree->RemoveEntity(entity->BVHNodePtr);
			entity->BVHNodePtr = nullptr;
		}
		else if (wasStatic)
		{
			entity->BVHNodePtr = nullptr;
		}

		std::unique_ptr<nCollisionEntity> moved = std::move(*sourceIt);
		sourceContainer.erase(sourceIt);

		moved->isStatic = targetStatic;

		nCollisionEntity* ptr = moved.get();
		targetContainer.push_back(std::move(moved));

		if (!targetStatic && insertNow)
		{
			m_DynamicTree->InsertEntity(ptr);
		}

		return true;
	}

	void nCollisionWorld::QueryAllOverlappingPairs(std::vector<std::pair<nCollisionEntity*, nCollisionEntity*>>& overlapEntities)
	{
		// Within the static tree, within the dynamic tree, then cross-tree pairs.
		if (m_StaticTree->GetRoot())
			nAABBTree<nCollisionEntity>::TraverseOverlaps(overlapEntities, m_StaticTree->GetRoot(), m_StaticTree->GetRoot());
		if (m_DynamicTree->GetRoot())
			nAABBTree<nCollisionEntity>::TraverseOverlaps(overlapEntities, m_DynamicTree->GetRoot(), m_DynamicTree->GetRoot());
		if (m_DynamicTree->GetRoot() && m_StaticTree->GetRoot())
			nAABBTree<nCollisionEntity>::TraverseCrossOverlaps(overlapEntities, m_DynamicTree->GetRoot(), m_StaticTree->GetRoot());
	}

	void nCollisionWorld::QueryOverlap(std::vector<std::pair<nCollisionEntity*, nCollisionEntity*>>& overlapEntities, const nCollisionEntity* entity)
	{
		if (!entity->BVHNodePtr) return;

		if (entity->isStatic)
		{
			if (m_StaticTree->GetRoot())
				nAABBTree<nCollisionEntity>::TraverseOverlaps(overlapEntities, entity->BVHNodePtr, m_StaticTree->GetRoot());
			if (m_DynamicTree->GetRoot())
				nAABBTree<nCollisionEntity>::TraverseCrossOverlaps(overlapEntities, entity->BVHNodePtr, m_DynamicTree->GetRoot());
		}
		else
		{
			if (m_StaticTree->GetRoot())
				nAABBTree<nCollisionEntity>::TraverseCrossOverlaps(overlapEntities, entity->BVHNodePtr, m_StaticTree->GetRoot());
			if (m_DynamicTree->GetRoot())
				nAABBTree<nCollisionEntity>::TraverseOverlaps(overlapEntities, entity->BVHNodePtr, m_DynamicTree->GetRoot());
		}
	}

} //namespace nNewton
