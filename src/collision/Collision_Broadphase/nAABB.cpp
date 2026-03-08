#include <nNewton/nCollision.hpp>

namespace nNewton
{
	struct nBVHNode
	{
		nAABB nodeAABB;
		std::unique_ptr <nBVHNode> leftChild;
		std::unique_ptr <nBVHNode> rightChild;
		nBVHNode* parent;
		nCollisionEntity* Entity;
		bool isLeaf() const { return Entity != nullptr; }
		nBVHNode() : leftChild(nullptr), rightChild(nullptr), Entity(nullptr),parent(nullptr)
		{
		}

	};
	class nAABBTree
	{
	public:
		void InsertEntity(nCollisionEntity* Ent_);
		void RemoveEntity(nBVHNode* leaf_);
		void RefitNode(nBVHNode* node_);
		void RefitUp(nBVHNode* leaf_);
		nBVHNode* FindBestSib(const nAABB& lAABB_);
		std::unique_ptr<nBVHNode> CreateLeafNode(nCollisionEntity* Ent_);

	private:
		std::unique_ptr <nBVHNode> staticTree;
		std::unique_ptr<nBVHNode> dynamicTree;
		std::unique_ptr<nBVHNode> root;
	};


	void nAABBTree::RefitNode(nBVHNode* node_)
	{

	}

	void nAABBTree::RefitUp(nBVHNode* leaf_)
	{
		auto* node = leaf_;
		while (node != nullptr)
		{
			if (!node->isLeaf()) 
			{
				node->nodeAABB = Merge(node->leftChild->nodeAABB,
					node->rightChild->nodeAABB);
			}
			node = node->parent;
		}
		
	}

	void nAABBTree::RemoveEntity(nBVHNode* leaf_)
	{
		if (leaf_->parent == nullptr)
		{
			//leaf is root 
			root = nullptr;
			return;
		}
		auto* pitaji = leaf_->parent;
		auto* dadaji = pitaji->parent;
		
		std::unique_ptr<nBVHNode> sibling;
		if (pitaji->rightChild.get() == leaf_)
		{
			sibling = std::move(pitaji->leftChild);
		}
		else
		{
			sibling = std::move(pitaji->rightChild);
		}

		if (dadaji == nullptr)
		{
			sibling->parent = nullptr;
			root = std::move(sibling);
			return;
		}

		sibling->parent = dadaji;
		if (dadaji->leftChild.get() == pitaji)
		{
			dadaji->leftChild = std::move(sibling);
		}
		else
		{
			dadaji->rightChild = std::move(sibling);
		}

		RefitUp(dadaji);
	}

	std::unique_ptr<nBVHNode> nAABBTree::CreateLeafNode(nCollisionEntity* Ent_)
	{
		auto leaf = std::make_unique<nBVHNode>();
		leaf->Entity = Ent_;
		leaf->nodeAABB = Ent_->marginAABB;
		return leaf;
	}

	void nAABBTree::InsertEntity(nCollisionEntity* Ent_)
	{
		auto Leaf = CreateLeafNode(Ent_);

		if(root == nullptr){
			root = std::move(Leaf);
			return;
		}
		nBVHNode* sibling = FindBestSib(Leaf->nodeAABB);
		auto* siblingPar = sibling->parent;


		std::unique_ptr<nBVHNode> NewParent = std::make_unique<nBVHNode>();
		NewParent->parent = sibling->parent;
		
		NewParent->nodeAABB = Merge(Leaf->nodeAABB, sibling->nodeAABB);
		
		Leaf->parent = NewParent.get();
		sibling->parent = NewParent.get();

		auto* NewParentRawPtr = NewParent.get();

		if (siblingPar == nullptr)
		{
			//sibling is root node
			
			NewParent->leftChild = std::move(root);
			NewParent->rightChild = std::move(Leaf);
			root = std::move(NewParent);

		}
		else
		{

			if (siblingPar->leftChild.get() == sibling)
			{
				NewParent->leftChild = std::move(siblingPar->leftChild);
				NewParent->rightChild = std::move(Leaf);
				siblingPar->leftChild = std::move(NewParent);
			}
			else
			{
				NewParent->rightChild = std::move(siblingPar->rightChild);
				NewParent->leftChild = std::move(Leaf);
				siblingPar->rightChild = std::move(NewParent);
			}
		}


		RefitUp(NewParentRawPtr->parent);

	}

	nBVHNode* nAABBTree::FindBestSib(const nAABB& lAABB_)
	{

	}
}
