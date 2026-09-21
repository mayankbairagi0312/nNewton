#pragma once

#include "nCollisionTypes.hpp"
#include "nTypes.hpp"
#include "nAABBTree.hpp"
#include"nAABBTraits.hpp"
#include "nDynamicAABBTree.hpp"
#include "nStaticAABBTree.hpp"
#include <memory>
#include "nRigidBody.hpp"
#include "nCollisionShapesPool.hpp"

namespace nNewton
{
	
	class nCollisionWorld
	{
	public:
		nCollisionWorld();


		template <class ShapeType>
		nCollisionEntity* CreateCollisionEntity(nEntity_ID& ID, nBodyType type, const nTransform& EntityTransform, const nVector3& vel,
			ShapeType&& Shape, bool insertNow = true);
		
		template<class ShapeType>
		bool RemoveCollisionEntity(nEntity_ID& ID, bool isStatic);
		void RemoveAll();
		void StepCollision();
		void BuildTrees();
		bool UpdateBodyType(
			nEntity_ID id,
			nBodyType newType,
			bool insertNow = true);
		void QueryAllOverlappingPairs(std::vector<std::pair<nCollisionEntity*,nCollisionEntity*>>& OverlapEntities);
		void QueryOverlap(std::vector<std::pair<nCollisionEntity*, nCollisionEntity*>>& OverlapEntities, const nCollisionEntity* A);
		nAABBTree<nCollisionEntity>* GetStaticTree() { return m_StaticTree.get(); }
		nAABBTree<nCollisionEntity>* GetDynamicTree() { return m_DynamicTree.get(); }
		bool RebuildBVH(bool isStatic);

		static nCollisionShapePool& GetColliderPool() {
			static nCollisionShapePool ColliderPool;
			return ColliderPool;
		}
	private:
		std::vector<std::unique_ptr<nCollisionEntity>> m_Static_Entities;
		std::vector<std::unique_ptr<nCollisionEntity>> m_Dynamic_Entities;

		std::unique_ptr<nAABBTree<nCollisionEntity>> m_DynamicTree;
		std::unique_ptr<nAABBTree<nCollisionEntity>> m_StaticTree;
		
		

		template<typename T>
		std::vector<T*> ToRawPtrs(std::vector<std::unique_ptr<T>>& entities)
		{
			std::vector<T*> raw;
			raw.reserve(entities.size());
			for (auto& e : entities)
				raw.push_back(e.get());
			return raw;
		}
	};

	
	template <class ShapeType>
	nCollisionEntity* nCollisionWorld::CreateCollisionEntity(nEntity_ID& ID, nBodyType type, const nTransform& EntityTransform, const nVector3& vel,
		ShapeType &&Shape, bool insertNow )
	{

		nCollisionEntity data;
		data.EntityID = ID;
		data.isStatic = type == nBodyType::Static ? true : false;
		data.EntityTransform = EntityTransform;
		data.vel = vel;
		data.EntityShape = GetColliderPool().createCollider(std::forward<ShapeType>(Shape));

		data.currentAABB = GetWorldAABB(data.EntityShape , EntityTransform, GetColliderPool());
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


	template<class ShapeType>
	bool nCollisionWorld::RemoveCollisionEntity(nEntity_ID& ID, bool isStatic)
	{
		auto& container = isStatic ? m_Static_Entities : m_Dynamic_Entities;


		auto it = std::find_if(container.begin(), container.end(),
			[&ID](const std::unique_ptr<nCollisionEntity>& ent) {
				return ent->EntityID == ID;
			});

		if (it == container.end()) return false;
		nCollisionEntity* ent = it->get();

		GetColliderPool().removeCollider<ShapeType>(ent->EntityShape);

		if (!isStatic && ent->BVHNodePtr) {
			m_DynamicTree->RemoveEntity(ent->BVHNodePtr);
			ent->BVHNodePtr = nullptr;
		}
		else
			ent->BVHNodePtr = nullptr;


		container.erase(it);

		return true;
	}
}