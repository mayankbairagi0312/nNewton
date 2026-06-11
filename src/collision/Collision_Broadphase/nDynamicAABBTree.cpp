
#include <nNewton/nAABBTree.hpp>
#include <algorithm>
#include <unordered_map>
#include <queue>

namespace nNewton
{
	
	class nDynamicAABBTree : public nAABBTree
	{
	private:
		void CollectLeaves(nBVHNode* node_, std::vector<nCollisionEntity*>& Leaves);
		nBVHNode* trav_down(nBVHNode* nodePtr, const nAABB& lAABB_);
		void RefitNode(nBVHNode* node_);
		void RefitUp(nBVHNode* leaf_);
		//void BuildReconstrQueue(nBVHNode* node_);
		nBVHNode* TreeletReconstrutSAH(nBVHNode* treelet_root);
		void PushNodeQue(nBVHNode* node);
		nBVHNode* PopNodeQue();
		void RemoveNodeQue(nBVHNode* node);
		void RemoveSubtreeQue(nBVHNode* node);

	public:
		nBVHStats CollectStats() const override {
			nBVHStats s = nAABBTree::CollectStats();
			s.queueSize = (int)QueSize;
			return s;
		}
		void Rebuild(std::vector<nCollisionEntity*>& entities) override;
		void Clear() override;
		void InsertEntity(nCollisionEntity* Ent_) override;
		void RemoveEntity(nBVHNode* leaf_) override;
		void UpdateEntity(nBVHNode* leaf) override;
		void TreeletStepRestructure() override;
		nBVHNode* FindBestSib(const nAABB& lAABB_);

	private:
		
		nBVHNode* m_QueTail;
		nBVHNode* m_QueHead;
		size_t QueSize = 0;

	};


	void nDynamicAABBTree::Rebuild(std::vector<nCollisionEntity*>& entities)
	{
		Clear();
		BuildAABBTree(entities);
	}

	void nDynamicAABBTree::Clear()
	{
		root.reset();
	}


	void nDynamicAABBTree::TreeletStepRestructure() 
	{
		nBVHNode* entry = PopNodeQue();
		if (!entry) return;

		if (entry->isLeaf())             return;
		if (!entry->isRefit) {
			PushNodeQue(entry);
			return;
		}
		entry->isRefit = false;

		auto* newEntry = TreeletReconstrutSAH(entry);

		PushNodeQue(newEntry);
	}

//=============
	nBVHNode* nDynamicAABBTree::TreeletReconstrutSAH(nBVHNode* treelet_root)
	{
		std::vector<nCollisionEntity*> leaves;

		CollectLeaves(treelet_root, leaves);

		if (leaves.size() < 2) return treelet_root;

		auto subtree = ConstructSAH(leaves, 0, (int)leaves.size());
		nBVHNode* new_root_ptr = subtree.get();
		//swap 

		nBVHNode* newparent = treelet_root->parent;
		subtree->parent = newparent;

		if (newparent == nullptr)
		{
			RemoveSubtreeQue(root.get());
			root = std::move(subtree);
		}
		else
		{
			if (newparent->leftChild.get() == treelet_root)
			{
				RemoveSubtreeQue(newparent->leftChild.get());
				newparent->leftChild = std::move(subtree);
			}
			else
			{
				RemoveSubtreeQue(newparent->rightChild.get());
				newparent->rightChild = std::move(subtree);
			}
			RefitUp(newparent);
		}
		return new_root_ptr;
	}

	void nDynamicAABBTree::PushNodeQue(nBVHNode* node)
	{
		if (node->inQueue)return;

		node->qPrev = m_QueTail;
		node->qNext = nullptr;
		node->inQueue = true;

		if (m_QueTail) m_QueTail->qNext = node;
		else m_QueHead = node;

		m_QueTail = node;
		QueSize++;
	}

	nBVHNode* nDynamicAABBTree::PopNodeQue()
	{
		if (!m_QueHead) return nullptr;

		nBVHNode* node = m_QueHead;

		m_QueHead = node->qNext;
		if (m_QueHead) { m_QueHead->qPrev = nullptr; }
		else m_QueTail = nullptr;

		node->qNext = nullptr;
		node->qPrev = nullptr;
		node->inQueue = false;
		QueSize--;
		return node;
	}

	void nDynamicAABBTree::RemoveNodeQue(nBVHNode* node)
	{
		if (!node->inQueue) return;
		if (node->qPrev) node->qPrev->qNext = node->qNext;
		else  m_QueHead = node->qNext;

		if (node->qNext) node->qNext->qPrev = node->qPrev;
		else m_QueTail = node->qPrev;

		node->qPrev = nullptr;
		node->qNext = nullptr;
		node->inQueue = false;
		QueSize--;
	}

	void nDynamicAABBTree::RemoveSubtreeQue(nBVHNode* node) {
		if (node == nullptr) return;

		if (node->inQueue) {
			RemoveNodeQue(node);
		}

		RemoveSubtreeQue(node->leftChild.get());
		RemoveSubtreeQue(node->rightChild.get());
	}

	void nDynamicAABBTree::CollectLeaves(nBVHNode* node_, std::vector<nCollisionEntity*>& Leaves)
	{
		if(node_ == nullptr ) return;
		if (node_->isLeaf())
		{
			Leaves.push_back(node_->Entity);
			return;
		}
		CollectLeaves(node_->leftChild.get(), Leaves);
		CollectLeaves(node_->rightChild.get(), Leaves);
	}

	void nDynamicAABBTree::UpdateEntity(nBVHNode* leaf)
	{
		if (!leaf->isLeaf()) { return; }
		
		leaf->Entity->currentAABB = leaf->Entity->EntityShape->getAABB(leaf->Entity->EntityTransform); nAABB tight = leaf->Entity->EntityShape->getAABB(leaf->Entity->EntityTransform);
		leaf->Entity->currentAABB = tight;

		nAABB expectedFat = Expand(tight, FAT_MARGIN);

		if (!Contains(leaf->nodeAABB, expectedFat) ||
			!Contains(expectedFat, leaf->nodeAABB))
		{
			auto* entity = leaf->Entity;
			entity->marginAABB = expectedFat;
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

			node->nodeAABB = Merge(node->leftChild->nodeAABB,
					node->rightChild->nodeAABB);
			if (!node->isRefit) {
				node->isRefit = true;
				PushNodeQue(node);
			}
			node = node->parent;
		}
		
	}

	void nDynamicAABBTree::RemoveEntity(nBVHNode* leaf_)
	{
	
		
		if (leaf_->parent == nullptr)
		{
			//leaf is root 
			if (root->inQueue) RemoveNodeQue(root.get());
			root = nullptr;
			return;
		}
		auto* Parent = leaf_->parent;
		auto* grandP = Parent->parent;

		if (Parent->inQueue) {
			RemoveNodeQue(Parent);
		}

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
	
}

