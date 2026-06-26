//nAABBTraits.hpp


#pragma once
#include "nCollisionTypes.hpp"
#include "nBVHNode.hpp"
#include <memory>
#include <vector>
#include <unordered_map>

namespace nNewton
{
	template <typename Entity>
	struct nBVHTraits 
	{
		static const size_t GetID(const Entity& entity);
		static const nAABB& GetAABB(const Entity& entity);
		static nAABB GetTightAABB(const Entity* e);
		static nAABB ComputeBounds(const std::vector<Entity*>& entities, int start, int end);
	};

	template <typename Entity>
	struct nAABBTreeTraits {
		static std::unique_ptr<nBVHNode<Entity>> CreateLeafNode(Entity* entity) {
			auto leaf = std::make_unique<nBVHNode<Entity>>();
			leaf->CollEntity = entity;
			
			nAABB tightAABB = nBVHTraits<Entity>::GetAABB(*entity);

			leaf->nodeAABB = Expand(tightAABB, FAT_MARGIN);
			SetBVHNodePtr(leaf->CollEntity,leaf.get());
			return leaf;
		}

		static void SetBVHNodePtr(Entity* ent, nBVHNode<Entity>* node);
		static void ClearBVHNodePtr(Entity* entity);
	};


	//-------------------------------------------- nCollisionEntity

	template <>
	inline const size_t  nBVHTraits<nCollisionEntity>::GetID(const nCollisionEntity& entity) {
		return entity.EntityID;
	}

	template <>
	inline const nAABB& nBVHTraits<nCollisionEntity>::GetAABB(const nCollisionEntity& e) {
		return e.currentAABB;
	}

	template<>
	inline nAABB nBVHTraits<nCollisionEntity>::GetTightAABB(const nCollisionEntity* e)
	{
		return e->EntityShape->getAABB(e->EntityTransform);
	}

	template<>
	inline nAABB nBVHTraits<nCollisionEntity>::ComputeBounds(const std::vector<nCollisionEntity*>& entities, int start, int end)
	{
		nAABB bounds = entities[start]->currentAABB;
		for (int i = start + 1; i < end; i++)
			bounds = Merge(bounds, entities[i]->currentAABB);
		return bounds;
	}

	template<>
	inline std::unique_ptr<nBVHNode<nCollisionEntity>> nAABBTreeTraits<nCollisionEntity>::CreateLeafNode(nCollisionEntity* entity)
	{
		auto leaf = std::make_unique<nBVHNode<nCollisionEntity>>();
		leaf->CollEntity = entity;
		leaf->nodeAABB = entity->marginAABB;
		entity->BVHNodePtr = leaf.get(); //store leaf ptr 
		return leaf;
	}

	template<>
	inline void nAABBTreeTraits<nCollisionEntity>::SetBVHNodePtr(nCollisionEntity* ent, nBVHNode<nCollisionEntity>* node)
	{
		ent->BVHNodePtr = node;
	}
	template<>
	inline void nAABBTreeTraits<nCollisionEntity>::ClearBVHNodePtr(nCollisionEntity* entity)
	{
		entity->BVHNodePtr = nullptr;
	}
	


	//--------------------------------------nBVHNode<nCollisionEntity>

	template<>
	inline nAABB nBVHTraits<nBVHNode<nCollisionEntity>>::ComputeBounds(const std::vector<nBVHNode<nCollisionEntity>*>& entities, int start, int end)
	{
		nAABB bounds = entities[start]->nodeAABB;
		for (int i = start + 1; i < end; i++)
			bounds = Merge(bounds, entities[i]->nodeAABB);
		return bounds;
	}
	template <>
	inline const nAABB& nBVHTraits<nBVHNode<nCollisionEntity>>::GetAABB(const nBVHNode<nCollisionEntity>& e) {
		return e.nodeAABB;
	}

	template <typename Entity>
	struct nAABBTreeTraits<nBVHNode<Entity>> {
		using EntityType = Entity;  

		static std::unique_ptr<nBVHNode<EntityType>> CreateLeafNode(nBVHNode<EntityType>* Node) {
			return std::unique_ptr<nBVHNode<EntityType>>(Node);
		}
	};
}