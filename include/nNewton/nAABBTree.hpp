#pragma once


#include "nCollisionTypes.hpp"
#include "nAABBTraits.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <algorithm>

namespace nNewton
{


	struct nBVHStats {
		int totalNodes = 0;
		int leafNodes = 0;
		int dirtyNodes = 0;
		int maxDepth = 0;
		float avgDepth = 0.f;
		int queueSize = 0;  // dynamic tree only
	};

	struct nSplit_SAH
	{
		int SPLIT_AXIS;
		int SPLIT_INDEX;
		float SPLIT_COST;
	};

	template<typename Entity>
	class nAABBTree
	{
	public:
		
		virtual ~nAABBTree() = default;

		void BuildAABBTree(std::vector<Entity*>& entities);
		virtual void UpdateEntity(nBVHNode<Entity>* leaf) { (void)leaf; }
		virtual void RemoveEntity(nBVHNode<Entity>* leaf_){ (void)leaf_; }
		virtual void TreeletStepRestructure() {}
		virtual void Rebuild(std::vector<Entity*>& entities) = 0;
		virtual void InsertEntity(Entity* Ent_){}

		virtual void Clear() = 0;

		void DebugDrawTree(std::function<void(const nAABB&, int, bool, bool)> callback);
		nBVHNode<Entity>* GetRoot() { return root.get(); }

		static void TraverseCrossOverlaps(std::vector<std::pair<Entity*, Entity*>>& OverlapEntities,
			const nBVHNode<Entity>* FirstTree, const nBVHNode<Entity>* SecTree);

		static void TraverseOverlaps(std::vector<std::pair<Entity*, Entity*>>& OverlapEntities,
			const nBVHNode<Entity>* a, const nBVHNode<Entity>* b);

		virtual nBVHStats CollectStats() const {
			return CollectStatsRecursive(root.get(), 0);
		}
	protected:
		template<typename Node>
		std::unique_ptr<nBVHNode<Entity>> ConstructSAH(std::vector<Node*>& entities, int start, int end);

		std::unique_ptr<nBVHNode<Entity>> root;

	private:
		template<typename Node>
		nSplit_SAH FindBestSplitSAH(std::vector<Node*>& entities, int start, int end);
		nBVHStats CollectStatsRecursive(nBVHNode<Entity>* node, int depth) const;
		void DebugDrawTreeNode(nBVHNode<Entity>* node, int depth, std::function<void(const nAABB&, int, bool, bool)> callback);
		
		
	};


	template<typename Entity>
	template<typename Node>
	nSplit_SAH nAABBTree<Entity>::FindBestSplitSAH(std::vector<Node*>& entities, int start, int end)
	{
		nSplit_SAH bestsplit = {};
		bestsplit.SPLIT_COST = FLT_MAX;
		bestsplit.SPLIT_AXIS = 0;


		nAABB ParAABB = nBVHTraits<Node>::ComputeBounds(entities, start, end);
		float ParAABB_SA = CalSurfaceArea(ParAABB);
		if (ParAABB_SA <= 1e-6f) ParAABB_SA = 1e-6f;
		int count = end - start;
		bestsplit.SPLIT_INDEX = start + count / 2;
		for (int AXI = 0; AXI < 3; ++AXI)
		{
			std::sort(entities.begin() + start,
				entities.begin() + end,
				[AXI](const Node* a, const Node* b)
				{
					return Centroid(nBVHTraits<Node>::GetAABB(*a), AXI) <
						Centroid(nBVHTraits<Node>::GetAABB(*b), AXI);
				});

			std::vector<nAABB>  leftAABB(count);
			std::vector<float> leftSA(count);

			leftAABB[0] = nBVHTraits<Node>::GetAABB(*entities[start]);
			leftSA[0] = CalSurfaceArea(leftAABB[0]);
			for (int i = 1; i < count; i++)
			{
				leftAABB[i] = Merge(leftAABB[i - 1], nBVHTraits<Node>::GetAABB(*entities[start + i]));
				leftSA[i] = CalSurfaceArea(leftAABB[i]);
			}


			std::vector<nAABB>  rightAABB(count);
			std::vector<float> rightSA(count);
			rightAABB[count - 1] = nBVHTraits<Node>::GetAABB(*entities[end - 1]);
			rightSA[count - 1] = CalSurfaceArea(rightAABB[count - 1]);
			for (int i = count - 2; i >= 0; i--)
			{
				rightAABB[i] = Merge(rightAABB[i + 1], nBVHTraits<Node>::GetAABB(*entities[start + i]));
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

	template<typename Entity>
	template<typename Node>
	std::unique_ptr<nBVHNode<Entity>> nAABBTree<Entity>::ConstructSAH(std::vector<Node*>& entities, int start, int end)
	{
		if (end - start == 1)
		{
			return nAABBTreeTraits<Node>::CreateLeafNode(entities[start]);
		}

		auto [axis, split, cost] = FindBestSplitSAH(entities, start, end);

		std::sort(entities.begin() + start, entities.begin() + end,
			[axis](const Node* a, const Node* b)
			{
				return Centroid(nBVHTraits<Node>::GetAABB(*a), axis) < Centroid(nBVHTraits<Node>::GetAABB(*b), axis);
			});


		auto node = std::make_unique<nBVHNode<Entity>>();

		node->leftChild = ConstructSAH<Node>(entities, start, split);
		node->rightChild = ConstructSAH<Node>(entities, split, end);

		node->leftChild->parent = node.get();
		node->rightChild->parent = node.get();

		node->nodeAABB = Merge(
			node->leftChild->nodeAABB,
			node->rightChild->nodeAABB
		);

		return node;

	}

	template<typename Entity>
	void nAABBTree<Entity>::BuildAABBTree(std::vector<Entity*>& entities)
	{
		root = ConstructSAH<Entity>(entities, 0, (int)entities.size());
	}

	template<typename Entity>
	void nAABBTree<Entity>::DebugDrawTreeNode(nBVHNode<Entity>* node, int depth, std::function<void(const nAABB&, int, bool, bool)> callback)
	{
		if (!node) return;
		bool isLeaf = node->isLeaf();
		callback(node->nodeAABB, depth, isLeaf, node->isRefit);
		DebugDrawTreeNode(node->leftChild.get(), depth + 1, callback);
		DebugDrawTreeNode(node->rightChild.get(), depth + 1, callback);
	}

	template<typename Entity>
	void nAABBTree<Entity>::DebugDrawTree(std::function<void(const nAABB&, int, bool, bool)> callback)
	{
		if (!root) return;
		DebugDrawTreeNode(root.get(), 0, callback);
	}

	template<typename Entity>
	nBVHStats nAABBTree<Entity>::CollectStatsRecursive(nBVHNode<Entity>* node, int depth) const
	{
		if (!node) return {};

		nBVHStats s;
		s.totalNodes = 1;
		s.maxDepth = depth;
		s.avgDepth = (float)depth;

		if (node->isRefit)    s.dirtyNodes = 1;
		if (node->isLeaf()) { s.leafNodes = 1; return s; }

		nBVHStats left = CollectStatsRecursive(node->leftChild.get(), depth + 1);
		nBVHStats right = CollectStatsRecursive(node->rightChild.get(), depth + 1);

		s.totalNodes += left.totalNodes + right.totalNodes;
		s.leafNodes += left.leafNodes + right.leafNodes;
		s.dirtyNodes += left.dirtyNodes + right.dirtyNodes;
		s.maxDepth = std::max({ s.maxDepth, left.maxDepth, right.maxDepth });
		s.avgDepth = (s.avgDepth + left.avgDepth + right.avgDepth) / 3.f;

		return s;
	}

	template<typename Entity>
	void nAABBTree<Entity>::TraverseOverlaps(std::vector<std::pair<Entity*, Entity*>>& OverlapEntities,
		const nBVHNode<Entity>* a, const nBVHNode<Entity>* b)
	{
		if (!a || !b) return;
		if (!Overlaps(a->nodeAABB, b->nodeAABB))
		{
			return;
		}

		if (a->isLeaf() && b->isLeaf())
		{
			if (a->CollEntity != b->CollEntity)
			{
				if (nBVHTraits<Entity>::GetID(*a->CollEntity) < nBVHTraits<Entity>::GetID(*b->CollEntity))
				{
					OverlapEntities.push_back({ a->CollEntity,b->CollEntity });
				}
			}
			return;
		}
		else if (!a->isLeaf() && !b->isLeaf())
		{
			TraverseOverlaps(OverlapEntities, a->leftChild.get(), b->leftChild.get());
			TraverseOverlaps(OverlapEntities, a->rightChild.get(), b->rightChild.get());
			TraverseOverlaps(OverlapEntities, a->leftChild.get(), b->rightChild.get());
			TraverseOverlaps(OverlapEntities, a->rightChild.get(), b->leftChild.get());
		}
		else if (a->isLeaf() && !b->isLeaf())
		{
			TraverseOverlaps(OverlapEntities, a, b->leftChild.get());
			TraverseOverlaps(OverlapEntities, a, b->rightChild.get());
		}

		else
		{
			TraverseOverlaps(OverlapEntities, a->leftChild.get(), b);
			TraverseOverlaps(OverlapEntities, a->rightChild.get(), b);
		}
	}

	template<typename Entity>
	void nAABBTree<Entity>::TraverseCrossOverlaps(std::vector<std::pair<Entity*, Entity*>>& OverlapEntities, const nBVHNode<Entity>* FirstTree,
		const nBVHNode<Entity>* SecTree)
	{
		if (!FirstTree || !SecTree) return;
		if (!Overlaps(FirstTree->nodeAABB, SecTree->nodeAABB))
		{
			return;
		}

		if (FirstTree->isLeaf() && SecTree->isLeaf())
		{
			if (FirstTree->CollEntity != SecTree->CollEntity) {
				OverlapEntities.push_back({ FirstTree->CollEntity,SecTree->CollEntity });
			}
			return;
		}
		else if (!FirstTree->isLeaf() && !SecTree->isLeaf())
		{
			TraverseCrossOverlaps(OverlapEntities, FirstTree->leftChild.get(), SecTree->leftChild.get());
			TraverseCrossOverlaps(OverlapEntities, FirstTree->rightChild.get(), SecTree->rightChild.get());
			TraverseCrossOverlaps(OverlapEntities, FirstTree->leftChild.get(), SecTree->rightChild.get());
			TraverseCrossOverlaps(OverlapEntities, FirstTree->rightChild.get(), SecTree->leftChild.get());
		}
		else if (FirstTree->isLeaf() && !SecTree->isLeaf())
		{
			TraverseCrossOverlaps(OverlapEntities, FirstTree, SecTree->leftChild.get());
			TraverseCrossOverlaps(OverlapEntities, FirstTree, SecTree->rightChild.get());
		}

		else
		{
			TraverseCrossOverlaps(OverlapEntities, FirstTree->leftChild.get(), SecTree);
			TraverseCrossOverlaps(OverlapEntities, FirstTree->rightChild.get(), SecTree);
		}

	}


	

}