
#include <nNewton/nAABBTree.hpp>
#include <algorithm>
namespace nNewton
{
	struct nSplit_SAH
	{
		int SPLIT_AXIS;
		int SPLIT_INDEX;
		int SPLIT_COST;
	};
	class nDynamicAABBTree : public nAABBTree
	{
	private:

		nBVHNode* trav_down(nBVHNode* nodePtr, const nAABB& lAABB_);
		float CalSurfaceArea(const nAABB& aabb);
		std::unique_ptr<nBVHNode> CreateLeafNode(nCollisionEntity* Ent_);
		void RefitNode(nBVHNode* node_);
		void RefitUp(nBVHNode* leaf_);
		std::unique_ptr<nBVHNode> ConstructSAH(std::vector<nCollisionEntity*>& entities, int start, int end);
		nSplit_SAH FindBestSplitSAH(std::vector<nCollisionEntity*>& entities, int start, int end);
		
	public:
		void InsertEntity(nCollisionEntity* Ent_);
		void RemoveEntity(nBVHNode* leaf_);
		void UpdateEntity(nBVHNode* leaf);
		nBVHNode* FindBestSib(const nAABB& lAABB_);
		void BuildAABBTree(std::vector<nCollisionEntity*>& entities) override;
	private:
		std::unique_ptr<nBVHNode> root;

	};
	

//=============



	void nDynamicAABBTree::BuildAABBTree(std::vector<nCollisionEntity*>& entities) 
	{
		root = ConstructSAH(entities, 0, entities.size());
	}

	std::unique_ptr<nBVHNode> nDynamicAABBTree::ConstructSAH(std::vector<nCollisionEntity*>& entities , int start , int end)
	{
		if(end - start == 1)
		{
				return CreateLeafNode(entities[start]);
		}

		auto [axis, split, cost] = FindBestSplitSAH(entities ,start, end);

		std::sort(entities.begin() + start, entities.begin() + end,
			[axis](const nCollisionEntity* a, const nCollisionEntity* b)
			{
				return Centroid(a->currentAABB, axis) < Centroid(b->currentAABB, axis);
			});


		std::unique_ptr<nBVHNode> node = std::make_unique<nBVHNode>();

		node->leftChild = ConstructSAH(entities, start, split);
		node->rightChild = ConstructSAH(entities, split, end);

		node->nodeAABB = Merge(
			node->leftChild->nodeAABB,
			node->rightChild->nodeAABB
		);

		return node;

	}
	
	

	nSplit_SAH nDynamicAABBTree::FindBestSplitSAH(std::vector<nCollisionEntity*>& entities, int start, int end)
	{
		nSplit_SAH bestsplit = {};
		bestsplit.SPLIT_COST = FLT_MAX;

		nAABB ParAABB = ComputeBounds(entities, start, end);
		float ParAABB_SA = CalSurfaceArea(ParAABB);

		int count = end - start;

		for (int AXI = 0; AXI < 3; ++AXI)
		{
			std::sort(entities.begin() + start,
				entities.begin() + end,
				[AXI](const nCollisionEntity* a, const nCollisionEntity* b)
				{
					return Centroid(a->currentAABB, AXI) <
						Centroid(b->currentAABB, AXI);
				});

			std::vector<nAABB>  leftAABB(count);
			std::vector<float> leftSA(count);

			leftAABB[0] = entities[start]->currentAABB;
			for (int i = 1; i < count; i++)
			{
				leftAABB[i] = Merge(leftAABB[i - 1],entities[start + i]->currentAABB);
				leftSA[i] = CalSurfaceArea(leftAABB[i]);
			}
			
			
			std::vector<nAABB>  rightAABB(count);
			std::vector<float> rightSA(count);
			rightAABB[count - 1] = entities[end - 1]->currentAABB;
			for (int i = count - 2; i >= 0; i--)
			{
				rightAABB[i] = Merge(rightAABB[i + 1],entities[start + i]->currentAABB);
				rightSA[i] = CalSurfaceArea(rightAABB[i]);
			}
			

			for (int i = 0; i < count - 1; i++)
			{
				int   leftCount = i + 1;
				int   rightCount = count - leftCount;
				float cost = (leftCount * leftSA[i] +
					rightCount * rightSA[i + 1]) / ParAABB_SA;

				if (cost < bestsplit.SPLIT_COST)
				{
					bestsplit.SPLIT_COST = cost;
					bestsplit.SPLIT_AXIS = AXI;
					bestsplit.SPLIT_INDEX = start + leftCount;
				}
			}
		}

		return bestsplit;
	}

	


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
		auto* nodePtr = root.get();
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
			return trav_down(nodePtr->rightChild.get(), lAABB_);
		}
		else
		{
			return trav_down(nodePtr->leftChild.get(), lAABB_);
		}
	}

	float nDynamicAABBTree::CalSurfaceArea(const nAABB& aabb)
	{
		auto ext = aabb.max - aabb.min;
		return 2.f*(ext.x * ext.y + ext.y * ext.z + ext.z * ext.x);
	}
	
}

