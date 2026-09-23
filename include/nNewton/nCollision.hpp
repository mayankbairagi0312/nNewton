#pragma once

#include "nCollisionTypes.hpp"
#include "nTypes.hpp"
#include "nAABBTree.hpp"
#include "nAABBTraits.hpp"
#include "nDynamicAABBTree.hpp"
#include "nStaticAABBTree.hpp"
#include "nRigidBody.hpp"
#include "nCollisionShapesPool.hpp"
#include <memory>

namespace nNewton
{

	class nCollisionWorld
	{
	public:
		// -- Constructors --
		nCollisionWorld();
		nCollisionWorld(const nCollisionWorld&) = delete;
		nCollisionWorld& operator=(const nCollisionWorld&) = delete;
		nCollisionWorld(nCollisionWorld&&) noexcept = default;
		nCollisionWorld& operator=(nCollisionWorld&&) & noexcept = default;
		~nCollisionWorld() = default;

		// -- Entity management --
		template <class ShapeType>
		nCollisionEntity* CreateCollisionEntity(nEntity_ID& id, nBodyType type, const nTransform& entityTransform, const nVector3& vel,
			ShapeType&& shape, bool insertNow = true);

		template<class ShapeType>
		bool RemoveCollisionEntity(nEntity_ID& id, bool isStatic);
		void RemoveAll();

		// -- Simulation / build --
		void StepCollision();
		void BuildTrees();
		bool RebuildBVH(bool isStatic);
		bool UpdateBodyType(nEntity_ID id, nBodyType newType, bool insertNow = true);

		// -- Queries --
		void QueryAllOverlappingPairs(std::vector<std::pair<nCollisionEntity*, nCollisionEntity*>>& overlapEntities);
		void QueryOverlap(std::vector<std::pair<nCollisionEntity*, nCollisionEntity*>>& overlapEntities, const nCollisionEntity* entity);

		// -- Accessors --
		nAABBTree<nCollisionEntity>* GetStaticTree() { return m_StaticTree.get(); }
		nAABBTree<nCollisionEntity>* GetDynamicTree() { return m_DynamicTree.get(); }

		static nCollisionShapePool& GetColliderPool() {
			static nCollisionShapePool colliderPool;
			return colliderPool;
		}

	private:
		std::vector<std::unique_ptr<nCollisionEntity>> m_StaticEntities;
		std::vector<std::unique_ptr<nCollisionEntity>> m_DynamicEntities;

		std::unique_ptr<nAABBTree<nCollisionEntity>> m_DynamicTree;
		std::unique_ptr<nAABBTree<nCollisionEntity>> m_StaticTree;

		template<typename T>
		std::vector<T*> ToRawPtrs(const std::vector<std::unique_ptr<T>>& entities) const
		{
			std::vector<T*> raw;
			raw.reserve(entities.size());
			for (const auto& e : entities)
				raw.push_back(e.get());
			return raw;
		}
	};

	template <class ShapeType>
	nCollisionEntity* nCollisionWorld::CreateCollisionEntity(nEntity_ID& id, nBodyType type, const nTransform& entityTransform, const nVector3& vel,
		ShapeType&& shape, bool insertNow)
	{
		nCollisionEntity data;
		data.EntityID = id;
		data.isStatic = (type == nBodyType::Static);
		data.EntityTransform = entityTransform;
		data.vel = vel;
		data.EntityShape = GetColliderPool().createCollider(std::forward<ShapeType>(shape));

		// Cache both the tight and fat AABBs for the BVH.
		data.currentAABB = GetWorldAABB(data.EntityShape, entityTransform, GetColliderPool());
		data.marginAABB = Expand(data.currentAABB, FAT_MARGIN);

		std::unique_ptr<nCollisionEntity> entity = std::make_unique<nCollisionEntity>(std::move(data));
		nCollisionEntity* ptr = entity.get();

		if (entity->isStatic)
		{
			m_StaticEntities.push_back(std::move(entity));
		}
		else
		{
			m_DynamicEntities.push_back(std::move(entity));
			if (insertNow)
				m_DynamicTree->InsertEntity(ptr);
		}

		return ptr;
	}

	template<class ShapeType>
	bool nCollisionWorld::RemoveCollisionEntity(nEntity_ID& id, bool isStatic)
	{
		std::vector<std::unique_ptr<nCollisionEntity>>& container = isStatic ? m_StaticEntities : m_DynamicEntities;

		auto it = std::find_if(container.begin(), container.end(),
			[&id](const std::unique_ptr<nCollisionEntity>& entity) {
				return entity->EntityID == id;
			});

		if (it == container.end()) return false;
		nCollisionEntity* entity = it->get();

		GetColliderPool().removeCollider<ShapeType>(entity->EntityShape);

		if (!isStatic && entity->BVHNodePtr) {
			m_DynamicTree->RemoveEntity(entity->BVHNodePtr);
			entity->BVHNodePtr = nullptr;
		}
		else
			entity->BVHNodePtr = nullptr;

		container.erase(it);

		return true;
	}
}
