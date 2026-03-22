#pragma once

#include "nCollisionTypes.hpp"
#include <memory>
#include <vector>

namespace nNewton
{

	constexpr float FAT_MARGIN = 0.01f;

	struct nBVHNode
	{
		nAABB nodeAABB;
		std::unique_ptr <nBVHNode> leftChild;
		std::unique_ptr <nBVHNode> rightChild;
		nBVHNode* parent;
		nCollisionEntity* Entity;
		bool isLeaf() const { return Entity != nullptr; }
		nBVHNode() : leftChild(nullptr), rightChild(nullptr), Entity(nullptr), parent(nullptr)
		{
		}

	};


	nAABB ComputeBounds(const std::vector<nCollisionEntity*>& entities, int start, int end)
	{
		nAABB bounds = entities[start]->currentAABB;
		for (int i = start + 1; i < end; i++)
			bounds = Merge(bounds, entities[i]->currentAABB);
		return bounds;
	}

	class nAABBTree
	{
	public:
		
		virtual ~nAABBTree() = default;
		virtual void BuildAABBTree(std::vector<nCollisionEntity*>& entities) = 0;
		virtual void QueryBroadPhase(const nAABB& aabb,
			std::vector<nCollisionEntity*>& res) const = 0;
		virtual nBVHNode* GetRoot() const = 0;
	private:
		//
	};

	

	

}