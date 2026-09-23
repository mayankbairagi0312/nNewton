// nAABBTraits.hpp — entity-to-BBV adaptation traits for the AABB trees.

#pragma once
#include "nCollisionTypes.hpp"
#include "nCollisionShapesPool.hpp"
#include "nBVHNode.hpp"
#include <memory>
#include <vector>
#include <unordered_map>

namespace nNewton
{
	template <typename Entity>
	struct nBVHTraits
	{
		static size_t GetID(const Entity& entity);
		static const nAABB& GetAABB(const Entity& entity);
		static nAABB GetTightAABB(const Entity* entity, nCollisionShapePool& pool);
		static nAABB ComputeBounds(const std::vector<Entity*>& entities, int start, int end);
	};

	template <typename Entity>
	struct nAABBTreeTraits {
		static std::unique_ptr<nBVHNode<Entity>> CreateLeafNode(Entity* entity) {
			std::unique_ptr<nBVHNode<Entity>> leaf = std::make_unique<nBVHNode<Entity>>();
			leaf->CollEntity = entity;

			const nAABB tightAABB = nBVHTraits<Entity>::GetAABB(*entity);

			leaf->nodeAABB = Expand(tightAABB, FAT_MARGIN);
			SetBVHNodePtr(leaf->CollEntity, leaf.get());
			return leaf;
		}

		static void SetBVHNodePtr(Entity* entity, nBVHNode<Entity>* node);
		static void ClearBVHNodePtr(Entity* entity);
	};

	//----------------------- nCollisionEntity specialization ------------

	template <>
	inline size_t nBVHTraits<nCollisionEntity>::GetID(const nCollisionEntity& entity) {
		return static_cast<size_t>(entity.EntityID);
	}

	template <>
	inline const nAABB& nBVHTraits<nCollisionEntity>::GetAABB(const nCollisionEntity& entity) {
		return entity.currentAABB;
	}

	template<>
	inline nAABB nBVHTraits<nCollisionEntity>::GetTightAABB(const nCollisionEntity* entity, nCollisionShapePool& pool)
	{
		return GetWorldAABB(entity->EntityShape, entity->EntityTransform, pool);
	}

	template<>
	inline nAABB nBVHTraits<nCollisionEntity>::ComputeBounds(const std::vector<nCollisionEntity*>& entities, int start, int end)
	{
		nAABB bounds = entities[start]->currentAABB;
		for (int i = start + 1; i < end; ++i)
			bounds = Merge(bounds, entities[i]->currentAABB);
		return bounds;
	}

	template<>
	inline std::unique_ptr<nBVHNode<nCollisionEntity>> nAABBTreeTraits<nCollisionEntity>::CreateLeafNode(nCollisionEntity* entity)
	{
		std::unique_ptr<nBVHNode<nCollisionEntity>> leaf = std::make_unique<nBVHNode<nCollisionEntity>>();
		leaf->CollEntity = entity;
		leaf->nodeAABB = entity->marginAABB;
		entity->BVHNodePtr = leaf.get(); // store leaf ptr
		return leaf;
	}

	template<>
	inline void nAABBTreeTraits<nCollisionEntity>::SetBVHNodePtr(nCollisionEntity* entity, nBVHNode<nCollisionEntity>* node)
	{
		entity->BVHNodePtr = node;
	}

	template<>
	inline void nAABBTreeTraits<nCollisionEntity>::ClearBVHNodePtr(nCollisionEntity* entity)
	{
		entity->BVHNodePtr = nullptr;
	}

	//-------------------------------------- nBVHNode<nCollisionEntity> specialization

	template<>
	inline nAABB nBVHTraits<nBVHNode<nCollisionEntity>>::ComputeBounds(const std::vector<nBVHNode<nCollisionEntity>*>& entities, int start, int end)
	{
		nAABB bounds = entities[start]->nodeAABB;
		for (int i = start + 1; i < end; ++i)
			bounds = Merge(bounds, entities[i]->nodeAABB);
		return bounds;
	}

	template <>
	inline const nAABB& nBVHTraits<nBVHNode<nCollisionEntity>>::GetAABB(const nBVHNode<nCollisionEntity>& entity) {
		return entity.nodeAABB;
	}

	template <typename Entity>
	struct nAABBTreeTraits<nBVHNode<Entity>> {
		using EntityType = Entity;

		// A pre-constructed node is adopted as-is (takes ownership).
		static std::unique_ptr<nBVHNode<EntityType>> CreateLeafNode(nBVHNode<EntityType>* node) {
			return std::unique_ptr<nBVHNode<EntityType>>(node);
		}
	};
}
