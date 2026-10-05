#pragma once

#include"nBVHNode.hpp"
#include "nAABBTree.hpp"
#include "nAABBTraits.hpp"
#include <algorithm>

namespace nNewton
{

	template<class Entity>
	class nDynamicAABBTree : public nAABBTree<Entity>
	{
	private:
		// -- Treelet --
		void CollectTreelet(nBVHNode<Entity>* start, std::vector<nBVHNode<Entity>*>& leaves, uint8_t maxLeaves);
		nBVHNode<Entity>* TreeletReconstructSAH(nBVHNode<Entity>* treeletRoot);

		// -- Traversal --
		nBVHNode<Entity>* TraverseDown(nBVHNode<Entity>* nodePtr, const nAABB& leafAABB);

		// -- Refit --
		void RefitNode(nBVHNode<Entity>* node);
		void RefitUp(nBVHNode<Entity>* leaf);

		// -- Intrusive refit queue --
		void PushNodeQue(nBVHNode<Entity>* node);
		nBVHNode<Entity>* PopNodeQue();
		void RemoveNodeQue(nBVHNode<Entity>* node);
		void RemoveSubtreeQue(nBVHNode<Entity>* node);

	public:
		// -- Constructors --
		nDynamicAABBTree() noexcept = default;
		nDynamicAABBTree(const nDynamicAABBTree&) = delete;
		nDynamicAABBTree& operator=(const nDynamicAABBTree&) = delete;
		nDynamicAABBTree(nDynamicAABBTree&&) noexcept = default;
		nDynamicAABBTree& operator=(nDynamicAABBTree&&) & noexcept = default;
		~nDynamicAABBTree() override = default;

		nBVHStats CollectStats() const override {
			nBVHStats s = nAABBTree<Entity>::CollectStats();
			s.queueSize = static_cast<int>(QueSize);
			return s;
		}

		// -- Build / update interface --
		void Rebuild(std::vector<Entity*>& entities) override;
		void Clear() override;

		void InsertEntity(Entity* entity) override;
		void RemoveEntity(nBVHNode<Entity>* leaf) override;
		void UpdateEntity(nBVHNode<Entity>* leaf) override;
		void TreeletStepRestructure() override;

		nBVHNode<Entity>* FindBestSibling(const nAABB& leafAABB);

	private:
		nBVHNode<Entity>* m_QueHead = nullptr;
		nBVHNode<Entity>* m_QueTail = nullptr;
		size_t QueSize = 0;
	};

	template<class Entity>
	void nDynamicAABBTree<Entity>::Rebuild(std::vector<Entity*>& entities)
	{
		Clear();
		nAABBTree<Entity>::BuildAABBTree(entities);
	}

	template<class Entity>
	void nDynamicAABBTree<Entity>::Clear()
	{
		m_QueHead = nullptr;
		m_QueTail = nullptr;
		QueSize = 0;
		nAABBTree<Entity>::root.reset();
	}

	template<class Entity>
	void nDynamicAABBTree<Entity>::TreeletStepRestructure()
	{
		nBVHNode<Entity>* entry = PopNodeQue();
		if (!entry) return;

		// Only internal nodes flagged for refit are restructured.
		if (entry->isLeaf())             return;
		if (!entry->isRefit) {
			PushNodeQue(entry);
			return;
		}
		entry->isRefit = false;

		TreeletReconstructSAH(entry);
	}

	//=============
	template<class Entity>
	nBVHNode<Entity>* nDynamicAABBTree<Entity>::TreeletReconstructSAH(nBVHNode<Entity>* treeletRoot)
	{
		std::vector<nBVHNode<Entity>*> leaves;

		CollectTreelet(treeletRoot, leaves, 8);

		if (leaves.size() < 2) return treeletRoot;

		// Detach the leaves from their parents (ownership moves to the vector).
		for (nBVHNode<Entity>* leaf : leaves)
		{
			if (leaf->parent)
			{
				if (leaf->parent->leftChild.get() == leaf) {
					leaf->parent->leftChild.release();
				}
				else if (leaf->parent->rightChild.get() == leaf) {
					leaf->parent->rightChild.release();
				}
			}
		}

		std::unique_ptr<nBVHNode<Entity>> subtree =
			nAABBTree<Entity>::template ConstructSAH<nBVHNode<Entity>>(leaves, 0, static_cast<int>(leaves.size()));
		nBVHNode<Entity>* newRootPtr = subtree.get();

		// Swap the rebuilt subtree in place of the old treelet root.
		nBVHNode<Entity>* newParent = treeletRoot->parent;
		subtree->parent = newParent;

		if (newParent == nullptr)
		{
			RemoveSubtreeQue(nAABBTree<Entity>::root.get());
			nAABBTree<Entity>::root = std::move(subtree);
		}
		else
		{
			if (newParent->leftChild.get() == treeletRoot)
			{
				RemoveSubtreeQue(newParent->leftChild.get());
				newParent->leftChild = std::move(subtree);
			}
			else
			{
				RemoveSubtreeQue(newParent->rightChild.get());
				newParent->rightChild = std::move(subtree);
			}
			RefitUp(newParent);
		}
		return newRootPtr;
	}

	template<class Entity>
	void nDynamicAABBTree<Entity>::PushNodeQue(nBVHNode<Entity>* node)
	{
		if (node->inQueue) return;

		node->qPrev = m_QueTail;
		node->qNext = nullptr;
		node->inQueue = true;

		if (m_QueTail) m_QueTail->qNext = node;
		else m_QueHead = node;

		m_QueTail = node;
		++QueSize;
	}

	template<class Entity>
	nBVHNode<Entity>* nDynamicAABBTree<Entity>::PopNodeQue()
	{
		if (!m_QueHead) return nullptr;

		nBVHNode<Entity>* node = m_QueHead;

		m_QueHead = node->qNext;
		if (m_QueHead) { m_QueHead->qPrev = nullptr; }
		else m_QueTail = nullptr;

		node->qNext = nullptr;
		node->qPrev = nullptr;
		node->inQueue = false;
		--QueSize;
		return node;
	}

	template<class Entity>
	void nDynamicAABBTree<Entity>::RemoveNodeQue(nBVHNode<Entity>* node)
	{
		if (!node->inQueue) return;
		if (node->qPrev) node->qPrev->qNext = node->qNext;
		else  m_QueHead = node->qNext;

		if (node->qNext) node->qNext->qPrev = node->qPrev;
		else m_QueTail = node->qPrev;

		node->qPrev = nullptr;
		node->qNext = nullptr;
		node->inQueue = false;
		--QueSize;
	}

	template<class Entity>
	void nDynamicAABBTree<Entity>::RemoveSubtreeQue(nBVHNode<Entity>* node)
	{
		if (node == nullptr) return;

		if (node->inQueue) {
			RemoveNodeQue(node);
		}

		RemoveSubtreeQue(node->leftChild.get());
		RemoveSubtreeQue(node->rightChild.get());
	}

	template<class Entity>
	void nDynamicAABBTree<Entity>::CollectTreelet(nBVHNode<Entity>* start, std::vector<nBVHNode<Entity>*>& nodes, uint8_t maxLeaves)
	{
		nodes.clear();
		if (start == nullptr) {
			return;
		}

		nodes.push_back(start);
		size_t index = 0;

		// BFS expansion until only leaves remain or the cap is hit.
		while (nodes.size() < maxLeaves && index < nodes.size())
		{
			nBVHNode<Entity>* curr = nodes[index];
			if (!curr->isLeaf()) {
				if (curr->leftChild)  nodes.push_back(curr->leftChild.get());
				if (curr->rightChild) nodes.push_back(curr->rightChild.get());

				nodes.erase(nodes.begin() + index);
			}
			else {
				++index;
			}
		}
	}

	template<class Entity>
	void nDynamicAABBTree<Entity>::UpdateEntity(nBVHNode<Entity>* leaf)
	{
		if (!leaf || !leaf->isLeaf()) { return; }

		const nAABB tight = nBVHTraits<Entity>::GetAABB(*(leaf->CollEntity));

		// Fat AABB no longer contains the entity: reinsert at the best position.
		if (!Contains(leaf->nodeAABB, tight))
		{
			Entity* entity = leaf->CollEntity;
			RemoveEntity(leaf);
			InsertEntity(entity);
		}
	}

	template<class Entity>
	void nDynamicAABBTree<Entity>::RefitNode(nBVHNode<Entity>* node)
	{
		if (node != nullptr && !node->isLeaf()) {
			node->nodeAABB = Merge(node->leftChild->nodeAABB,
				node->rightChild->nodeAABB);
		}
	}

	template<class Entity>
	void nDynamicAABBTree<Entity>::RefitUp(nBVHNode<Entity>* leaf)
	{
		nBVHNode<Entity>* node = leaf;
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

	template<class Entity>
	void nDynamicAABBTree<Entity>::RemoveEntity(nBVHNode<Entity>* leaf)
	{
		if (!leaf) return;

		if (leaf->CollEntity)
			nAABBTreeTraits<Entity>::ClearBVHNodePtr(leaf->CollEntity);

		if (leaf->parent == nullptr)
		{
			if (nAABBTree<Entity>::root->inQueue) RemoveNodeQue(nAABBTree<Entity>::root.get());
			nAABBTree<Entity>::root = nullptr;
			return;
		}

		nBVHNode<Entity>* parent = leaf->parent;
		nBVHNode<Entity>* grandParent = parent->parent;

		RemoveNodeQue(leaf);
		if (parent->inQueue) {
			RemoveNodeQue(parent);
		}

		// Replace the parent with the remaining sibling.
		std::unique_ptr<nBVHNode<Entity>> sibling;
		if (parent->rightChild.get() == leaf)
		{
			sibling = std::move(parent->leftChild);
		}
		else
		{
			sibling = std::move(parent->rightChild);
		}

		if (grandParent == nullptr)
		{
			sibling->parent = nullptr;
			nAABBTree<Entity>::root = std::move(sibling);
			return;
		}

		sibling->parent = grandParent;
		if (grandParent->leftChild.get() == parent)
		{
			grandParent->leftChild = std::move(sibling);
		}
		else
		{
			grandParent->rightChild = std::move(sibling);
		}

		RefitUp(grandParent);
	}

	template<class Entity>
	void nDynamicAABBTree<Entity>::InsertEntity(Entity* entity)
	{
		std::unique_ptr<nBVHNode<Entity>> leaf = nAABBTreeTraits<Entity>::CreateLeafNode(entity);

		if (nAABBTree<Entity>::root == nullptr) {
			nAABBTree<Entity>::root = std::move(leaf);
			return;
		}

		nBVHNode<Entity>* sibling = FindBestSibling(leaf->nodeAABB);
		nBVHNode<Entity>* siblingParent = sibling->parent;

		std::unique_ptr<nBVHNode<Entity>> newParent = std::make_unique<nBVHNode<Entity>>();

		nBVHNode<Entity>* newParentPtr = newParent.get();
		newParent->parent = sibling->parent;
		newParent->nodeAABB = Merge(leaf->nodeAABB, sibling->nodeAABB);

		leaf->parent = newParent.get();
		sibling->parent = newParent.get();

		if (siblingParent == nullptr)
		{
			// Sibling is the root node.
			newParent->leftChild = std::move(nAABBTree<Entity>::root);
			newParent->rightChild = std::move(leaf);
			nAABBTree<Entity>::root = std::move(newParent);
		}
		else
		{
			if (siblingParent->leftChild.get() == sibling)
			{
				newParent->leftChild = std::move(siblingParent->leftChild);
				newParent->rightChild = std::move(leaf);
				siblingParent->leftChild = std::move(newParent);
			}
			else
			{
				newParent->rightChild = std::move(siblingParent->rightChild);
				newParent->leftChild = std::move(leaf);
				siblingParent->rightChild = std::move(newParent);
			}
		}

		RefitUp(newParentPtr);
	}

	template<class Entity>
	nBVHNode<Entity>* nDynamicAABBTree<Entity>::FindBestSibling(const nAABB& leafAABB)
	{
		nBVHNode<Entity>* nodePtr = nAABBTree<Entity>::root.get();
		return TraverseDown(nodePtr, leafAABB);
	}

	template<class Entity>
	nBVHNode<Entity>* nDynamicAABBTree<Entity>::TraverseDown(nBVHNode<Entity>* nodePtr, const nAABB& leafAABB)
	{
		if (!nodePtr) return nullptr;
		if (nodePtr->isLeaf()) return nodePtr;

		const nAABB mergedAABB_L = Merge(nodePtr->leftChild->nodeAABB, leafAABB);
		const float leftCost = CalSurfaceArea(mergedAABB_L) - CalSurfaceArea(nodePtr->leftChild->nodeAABB);

		const nAABB mergedAABB_R = Merge(nodePtr->rightChild->nodeAABB, leafAABB);
		const float rightCost = CalSurfaceArea(mergedAABB_R) - CalSurfaceArea(nodePtr->rightChild->nodeAABB);

		if (rightCost < leftCost)
		{
			return TraverseDown(nodePtr->rightChild.get(), leafAABB);
		}
		return TraverseDown(nodePtr->leftChild.get(), leafAABB);
	}
}
