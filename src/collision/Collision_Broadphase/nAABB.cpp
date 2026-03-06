#include <nNewton/nCollision.hpp>

namespace nNewton
{
	struct nBVHNode
	{
		nAABB nodeAABB;
		std::unique_ptr <nBVHNode> leftChild;
		std::unique_ptr <nBVHNode> rightChild;
		nCollisionEntity* Entity;
		bool isLeaf() const { return Entity != nullptr; }
		nBVHNode() : leftChild(nullptr), rightChild(nullptr), Entity(nullptr)
		{
		}

	};
	class nAABBTree
	{
	public:
		void InsertEntity(nCollisionEntity* Ent_);
		void RemoveEntity(nBVHNode* leaf_);
		void Refit(nBVHNode* node_);
		void RefitUpwards(nBVHNode* leaf_);
		nBVHNode* FindBestSibling(const nAABB& lAABB_);
		nBVHNode* CreateLeafNode(nCollisionEntity* Ent_);

	private:
		std::unique_ptr <nBVHNode> staticTree;
		std::unique_ptr<nBVHNode> dynamicTree;
		nBVHNode* root;
	};

	void nAABBTree::InsertEntity(nCollisionEntity* Ent_)
	{
		nBVHNode* Leaf = CreateLeafNode(Ent_);

		if(root == nullptr){
			root = Leaf;
			return;
		}
		nBVHNode* sibling = FindBestSibling(Leaf->nodeAABB);
		
	}

}
