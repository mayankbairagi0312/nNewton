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
		void CollectTreelet(nBVHNode<Entity>* start, std::vector<nBVHNode<Entity>*>& Leaves, uint8_t maxLeaves);
		nBVHNode<Entity>* trav_down(nBVHNode<Entity>* nodePtr, const nAABB& lAABB_);
		void RefitNode(nBVHNode<Entity>* node_);
		void RefitUp(nBVHNode<Entity>* leaf_);
		nBVHNode<Entity>* TreeletReconstrutSAH(nBVHNode<Entity>* treelet_root);
		void PushNodeQue(nBVHNode<Entity>* node);
		nBVHNode<Entity>* PopNodeQue();
		void RemoveNodeQue(nBVHNode<Entity>* node);
		void RemoveSubtreeQue(nBVHNode<Entity>* node);

	public:
		nBVHStats CollectStats() const override {
			nBVHStats s = nAABBTree<Entity>::CollectStats();
			s.queueSize = (int)QueSize;
			return s;
		}
		void Rebuild(std::vector<Entity*>& entities) override;
		void Clear() override;

		void InsertEntity(Entity* Ent_) override;
		void RemoveEntity(nBVHNode<Entity>* leaf_) override;
		void UpdateEntity(nBVHNode<Entity>* leaf) override;
		void TreeletStepRestructure() override;
		nBVHNode<Entity>* FindBestSib(const nAABB& lAABB_);

	private:

		nBVHNode<Entity>* m_QueTail;
		nBVHNode<Entity>* m_QueHead;
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

		if (entry->isLeaf())             return;
		if (!entry->isRefit) {
			PushNodeQue(entry);
			return;
		}
		entry->isRefit = false;

		TreeletReconstrutSAH(entry);
	}

	//=============
	template<class Entity>
	nBVHNode<Entity>* nDynamicAABBTree<Entity>::TreeletReconstrutSAH(nBVHNode<Entity>* treelet_root)
	{
		std::vector<nBVHNode<Entity>*> leaves;

		CollectTreelet(treelet_root, leaves, 8);

		if (leaves.size() < 2) return treelet_root;

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

		auto subtree = nAABBTree<Entity>::template ConstructSAH<nBVHNode<Entity>>(leaves, 0, (int)leaves.size());
		nBVHNode<Entity>* new_root_ptr = subtree.get();

		//swap 
		nBVHNode<Entity>* newparent = treelet_root->parent;
		subtree->parent = newparent;

		if (newparent == nullptr)
		{
			RemoveSubtreeQue(nAABBTree<Entity>::root.get());
			nAABBTree<Entity>::root = std::move(subtree);
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

	template<class Entity>
	void nDynamicAABBTree<Entity>::PushNodeQue(nBVHNode<Entity>* node)
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
		QueSize--;
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
		QueSize--;
	}

	template<class Entity>
	void nDynamicAABBTree<Entity>::RemoveSubtreeQue(nBVHNode<Entity>* node) {
		if (node == nullptr) return;

		if (node->inQueue) {
			RemoveNodeQue(node);
		}

		RemoveSubtreeQue(node->leftChild.get());
		RemoveSubtreeQue(node->rightChild.get());
	}

	template<class Entity>
	void nDynamicAABBTree<Entity>::CollectTreelet(nBVHNode<Entity>* start, std::vector<nBVHNode<Entity>*>& Node, uint8_t maxLeaves)
	{
		Node.clear();
		if (start == nullptr) {
			return;
		}

		Node.push_back(start);
		size_t index = 0;


		while (Node.size() < maxLeaves && index < Node.size())
		{
			nBVHNode<Entity>* curr = Node[index];
			if (!curr->isLeaf()) {
				if (curr->leftChild)  Node.push_back(curr->leftChild.get());
				if (curr->rightChild) Node.push_back(curr->rightChild.get());

				Node.erase(Node.begin() + index);
			}
			else {
				index++;
			}
		}
	}

	template<class Entity>
	void nDynamicAABBTree<Entity>::UpdateEntity(nBVHNode<Entity>* leaf)
	{
		if (!leaf->isLeaf()) { return; }

		nAABB tight = nBVHTraits<Entity>::GetAABB(*(leaf->CollEntity));

		if (!Contains(leaf->nodeAABB, tight))
		{
			
			auto* entity = leaf->CollEntity;
			RemoveEntity(leaf);
			InsertEntity(entity);
		}

	}

	template<class Entity>
	void nDynamicAABBTree<Entity>::RefitNode(nBVHNode<Entity>* node_)
	{
		if (node_ != nullptr && !node_->isLeaf()) {
			node_->nodeAABB = Merge(node_->leftChild->nodeAABB,
				node_->rightChild->nodeAABB);
		}
	}

	template<class Entity>
	void nDynamicAABBTree<Entity>::RefitUp(nBVHNode<Entity>* leaf_)
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

	template<class Entity>
	void nDynamicAABBTree<Entity>::RemoveEntity(nBVHNode<Entity>* leaf_)
	{
		if (!leaf_) return;

		if (leaf_->CollEntity)
			nAABBTreeTraits<Entity>::ClearBVHNodePtr(leaf_->CollEntity);
		
		if (leaf_->parent == nullptr)
		{
			//leaf is root 
			if (nAABBTree<Entity>::root->inQueue) RemoveNodeQue(nAABBTree<Entity>::root.get());
			nAABBTree<Entity>::root = nullptr;
			return;
		}
		auto* Parent = leaf_->parent;
		auto* grandP = Parent->parent;

		RemoveNodeQue(leaf_);
		if (Parent->inQueue) {
			RemoveNodeQue(Parent);
		}

		std::unique_ptr<nBVHNode<Entity>> sibling;
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
			nAABBTree<Entity>::root = std::move(sibling);
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

	template<class Entity>
	void nDynamicAABBTree<Entity>::InsertEntity(Entity* Ent_)
	{
		auto Leaf = nAABBTreeTraits<Entity>::CreateLeafNode(Ent_);


		if (nAABBTree<Entity>::root == nullptr) {
			nAABBTree<Entity>::root = std::move(Leaf);
			return;
		}
		nBVHNode<Entity>* sibling = FindBestSib(Leaf->nodeAABB);
		auto* siblingPar = sibling->parent;


		auto NewParent = std::make_unique<nBVHNode<Entity>>();
		NewParent->parent = sibling->parent;

		NewParent->nodeAABB = Merge(Leaf->nodeAABB, sibling->nodeAABB);

		Leaf->parent = NewParent.get();
		sibling->parent = NewParent.get();

		auto* NewParentRawPtr = NewParent.get();

		if (siblingPar == nullptr)
		{
			//sibling is root node

			NewParent->leftChild = std::move(nAABBTree<Entity>::root);
			NewParent->rightChild = std::move(Leaf);
			nAABBTree<Entity>::root = std::move(NewParent);

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

	template<class Entity>
	nBVHNode<Entity>* nDynamicAABBTree<Entity>::FindBestSib(const nAABB& lAABB_)
	{
		auto* nodePtr = nAABBTree<Entity>::root.get();
		return trav_down(nodePtr, lAABB_);

	}

	template<class Entity>
	nBVHNode<Entity>* nDynamicAABBTree<Entity>::trav_down(nBVHNode<Entity>* nodePtr, const nAABB& lAABB_)
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