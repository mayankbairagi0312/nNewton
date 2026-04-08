
#include <nNewton/nAABBTree.hpp>
#include <algorithm>
#include <unordered_map>
#include <queue>

namespace nNewton
{
	//for treelet reconstruction
	struct TreeletNode
	{
		nBVHNode* node;
		uint64_t refPoint;
	};


	class nDynamicAABBTree : public nAABBTree
	{
	private:
		void CollectLeaves(nBVHNode* node_, std::vector<nCollisionEntity*>& Leaves);
		nBVHNode* trav_down(nBVHNode* nodePtr, const nAABB& lAABB_);
		void RefitNode(nBVHNode* node_);
		void RefitUp(nBVHNode* leaf_);
		void BuildReconstrQueue(nBVHNode* node_);
		void TreeletReconstrutSAH(nBVHNode* treelet_root);

	public:
		void Rebuild(std::vector<nCollisionEntity*>& entities) override;
		void Clear() override;
		void InsertEntity(nCollisionEntity* Ent_);
		void RemoveEntity(nBVHNode* leaf_);
		void UpdateEntity(nBVHNode* leaf) override;
		void TreeletStepRestructure() override;
		nBVHNode* FindBestSib(const nAABB& lAABB_);

	private:
		
		std::deque<TreeletNode> m_ReconstructQueue;

	};


	void nDynamicAABBTree::Rebuild(std::vector<nCollisionEntity*>& entities)
	{
		Clear();
		BuildAABBTree(entities);
		BuildReconstrQueue(root.get());
	}

	void nDynamicAABBTree::Clear()
	{
		m_ReconstructQueue.clear();
		root.reset();
	}


	void nDynamicAABBTree::TreeletStepRestructure() {

		if (m_ReconstructQueue.empty()) return;

		TreeletNode entry = m_ReconstructQueue.front();
		m_ReconstructQueue.pop_front();

		if (entry.node->refPoint != entry.refPoint) return;
		if (entry.node->isLeaf())             return;
		if (!entry.node->isRefit) {
			m_ReconstructQueue.push_back({ entry.node, entry.node->refPoint});
			return;
		}
		entry.node->refPoint++;
		entry.node->isRefit = false;

		TreeletReconstrutSAH(entry.node);

		m_ReconstructQueue.push_back({ entry.node, entry.node->refPoint });
	}

	void nDynamicAABBTree::BuildReconstrQueue(nBVHNode* node_)
	{
		if (!node_ || node_->isLeaf()) return;
		m_ReconstructQueue.push_back({ node_, node_->refPoint });
		BuildReconstrQueue(node_->leftChild.get());                       // recurse left
		BuildReconstrQueue(node_->rightChild.get());
	}

//=============
	void nDynamicAABBTree::TreeletReconstrutSAH(nBVHNode* treelet_root)
	{
		std::vector<nCollisionEntity*> leaves;

		CollectLeaves(treelet_root, leaves);

		if (leaves.size() < 2) return;

		auto subtree = ConstructSAH(leaves, 0, (int)leaves.size());

		//swap 

		nBVHNode* newparent = treelet_root->parent;
		subtree->parent = newparent;

		if (newparent == nullptr)
		{
			root = std::move(subtree);
		}
		else
		{
			if (newparent->leftChild.get() == treelet_root)
			{
				newparent->leftChild = std::move(subtree);
			}
			else
			{
				newparent->rightChild = std::move(subtree);
			}
			RefitUp(newparent);
		}

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

			node->nodeAABB = Merge(node->leftChild->nodeAABB,
					node->rightChild->nodeAABB);
			if (!node->isRefit) {
				node->isRefit = true;
				m_ReconstructQueue.push_back({ node, node->refPoint }); 
			}
			node = node->parent;
		}
		
	}

	void nDynamicAABBTree::RemoveEntity(nBVHNode* leaf_)
	{
		leaf_->refPoint++;
		
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

