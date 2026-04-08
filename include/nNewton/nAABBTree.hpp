#pragma once

#include "nCollisionTypes.hpp"
#include <memory>
#include <vector>
#include <unordered_map>

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

		uint64_t refPoint = 0;
		bool isRefit = false;

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
	
	struct nSplit_SAH
	{
		int SPLIT_AXIS;
		int SPLIT_INDEX;
		float SPLIT_COST;
	};

	class nAABBTree
	{
	public:
		
		virtual ~nAABBTree() = default;
		void BuildAABBTree(std::vector<nCollisionEntity*>& entities);
		void QueryBroadPhase(const nAABB& aabb,
			std::vector<nCollisionEntity*>& res);

		virtual void UpdateEntity(nBVHNode* leaf) {}
		virtual void TreeletStepRestructure() {}
		virtual void Rebuild(std::vector<nCollisionEntity*>& entities) = 0;
		
		virtual void Clear() = 0;

	protected: 

		std::unique_ptr<nBVHNode> ConstructSAH(std::vector<nCollisionEntity*>& entities, int start, int end);

		std::unique_ptr<nBVHNode> CreateLeafNode(nCollisionEntity* Ent_);
		std::unique_ptr<nBVHNode> root;
		nBVHNode* GetRoot() { return root.get(); }
	private:
		nSplit_SAH FindBestSplitSAH(std::vector<nCollisionEntity*>& entities, int start, int end);
		
		
	};

	

	

}