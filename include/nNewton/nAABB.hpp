#pragma once

namespace nNewton
{

	constexpr float FAT_MARGIN = 0.01;

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

	class nAABBTree
	{
	private:
		nBVHNode* trav_down(const nBVHNode* nodePtr, const nAABB& lAABB_);
		float CalSurfaceArea(const nAABB& aabb);
		std::unique_ptr<nBVHNode> CreateLeafNode(nCollisionEntity* Ent_);
		void RefitNode(nBVHNode* node_);
		void RefitUp(nBVHNode* leaf_);
	public:
		void InsertEntity(nCollisionEntity* Ent_);
		void RemoveEntity(nBVHNode* leaf_);
		void UpdateEntity(nBVHNode* leaf);
		nBVHNode* FindBestSib(const nAABB& lAABB_);
		void buildAABBTree(const std::vector<nCollisionEntity>& entities);
	private:
		std::unique_ptr<nBVHNode> root;
	}

}