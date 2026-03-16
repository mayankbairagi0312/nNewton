
#include <nNewton/nAABBTree.hpp>

namespace nNewton
{
	class nDynamicAABBTree : public nAABBTree
	{
	private:
		nBVHNode* trav_down(const nBVHNode* nodePtr, const nAABB& lAABB_);
		float CalSurfaceArea(const nAABB& aabb);
		std::unique_ptr<nBVHNode> CreateLeafNode(nCollisionEntity* Ent_);
		void RefitNode(nBVHNode* node_);
		void RefitUp(nBVHNode* leaf_);
		nBVHNode* ConstructSAH(const std::vector<nCollisionEntity>& entities);
	public:
		void InsertEntity(nCollisionEntity* Ent_);
		void RemoveEntity(nBVHNode* leaf_);
		void UpdateEntity(nBVHNode* leaf);
		nBVHNode* FindBestSib(const nAABB& lAABB_);
		void BuildAABBTree(const std::vector<nCollisionEntity>& entities) override;
	private:
		std::unique_ptr<nBVHNode> root;
	}
	A

//=============



	/*void nDynamicAABBTree::BuildAABBTree(const std::vector<nCollisionEntity>& entities) override
	{
		root = ConstructSAH(entities);
	}*/

	/*nBVHNode* nDynamicAABBTree::ConstructSAH(const std::vector<nCollisionEntity>& entities)
	{

	}*/

	void nDynamicAABBTree::UpdateEntity(nBVHNode* leaf)
	{
		if (!leaf->isLeaf()) { return; }
		
		leaf->Entity->currentAABB = leaf->Entity->EntityShape->getAABB(leaf->Entity->EntityTransform);

		if (!Contains(leaf->nodeAABB, leaf->Entity->currentAABB))
		{
			auto* entity = leaf->Entity;
			entity->marginAABB = Expand(entity->currentAABB, FAT_MARGIN);

			RemoveEntity(leaf);
			InsertEntity(entity);
		}
	}

	void nDynamicAABBTree::RefitNode(nBVHNode* node_)
	{
		if (node_ != nullptr && !node_->isLeaf()) {
			node_->nodeAABB = Merge(node_->leftChild->nodeAABB,
				node_->rightChild->nodeAABB);
		}
	}

	void nDynamicAABBTree::RefitUp(nBVHNode* leaf_)
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

	void nDynamicAABBTree::RemoveEntity(nBVHNode* leaf_)
	{
		if (leaf_->parent == nullptr)
		{
			//leaf is root 
			root = nullptr;
			return;
		}
		auto* Parent = leaf_->parent;
		auto* grandP = Parent->parent;
		
		std::unique_ptr<nBVHNode> sibling;
		if (Parent->rightChild.get() == leaf_)
		{
			sibling = std::move(Parent->leftChild);
		}
		else
		{
			sibling = std::move(Parent->rightChild);
		}

		if (grandP == nullptr)
		{
			sibling->parent = nullptr;
			root = std::move(sibling);
			return;
		}

		sibling->parent = grandP;
		if (grandP->leftChild.get() == Parent)
		{
			grandP->leftChild = std::move(sibling);
		}
		else
		{
			grandP->rightChild = std::move(sibling);
		}

		RefitUp(grandP);
	}

	std::unique_ptr<nBVHNode> nDynamicAABBTree::CreateLeafNode(nCollisionEntity* Ent_)
	{
		auto leaf = std::make_unique<nBVHNode>();
		leaf->Entity = Ent_;
		leaf->nodeAABB = Ent_->marginAABB;
		return leaf;
	}

	void nDynamicAABBTree::InsertEntity(nCollisionEntity* Ent_)
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

	nBVHNode* nDynamicAABBTree::FindBestSib(const nAABB& lAABB_)
	{
		const auto* nodePtr = root.get();
		return trav_down(nodePtr,lAABB_);
		
	}

	nBVHNode* nDynamicAABBTree::trav_down(nBVHNode* nodePtr, const nAABB& lAABB_)
	{
		if (!nodePtr) return nullptr;
		if (nodePtr->isLeaf()) return nodePtr;

		nAABB merAABB_L = Merge(nodePtr->leftChild->nodeAABB, lAABB_);
		auto LeftCost = CalSurfaceArea(merAABB_L) - CalSurfaceArea(nodePtr->leftChild->nodeAABB);

		nAABB merAABB_R = Merge(nodePtr->rightChild->nodeAABB, lAABB_);
		auto RightCost = CalSurfaceArea(merAABB_R) - CalSurfaceArea(nodePtr->rightChild->nodeAABB);


		if (RightCost < LeftCost)
		{
			return trav_down(nodePtr->rightChild, lAABB_);
		}
		else
		{
			return trav_down(nodePtr->leftChild,lAABB_)
		}
	}

	float nDynamicAABBTree::CalSurfaceArea(const nAABB& aabb)
	{
		auto ext = aabb.max - aabb.min;
		return 2.f*(ext.x * ext.y + ext.y * ext.z + ext.z * ext.x);
	}
	
}

