#include <nNewton/nCollision.hpp>
#include <nNewton/nCollisionTypes.hpp>
#include <nNewton/nAABB.hpp>

namespace nNewton
{
	void nAABBTree::buildAABBTree(const std::vector<nCollisionEntity>& entities)
	{
		for (const nCollisionEntity& ent : entities)
		{
			
		}
	}


	void nAABBTree::UpdateEntity(nBVHNode* leaf)
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

	void nAABBTree::RefitNode(nBVHNode* node_)
	{
		node_->nodeAABB = Merge(node_->leftChild->nodeAABB,
			node_->rightChild->nodeAABB);
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
		const auto* nodePtr = root.get();
		return trav_down(nodePtr,lAABB_);
		
	}

	nBVHNode* trav_down(nBVHNode* nodePtr, const nAABB& lAABB_)
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

	float nAABBTree::CalSurfaceArea(const nAABB& aabb)
	{
		auto ext = aabb.max - aabb.min;
		return 2.f*(ext.x * ext.y + ext.y * ext.z + ext.z * ext.x);
	}
	
}

