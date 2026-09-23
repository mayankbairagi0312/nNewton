#pragma once

#include "nCollisionTypes.hpp"
#include "nAABBTraits.hpp"
#include <cfloat>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <algorithm>

namespace nNewton
{
	// Aggregate statistics for a BVH.
	struct nBVHStats {
		int totalNodes = 0;
		int leafNodes = 0;
		int dirtyNodes = 0;
		int maxDepth = 0;
		float avgDepth = 0.0f;
		int queueSize = 0;  // dynamic tree only
	};

	// Result of a SAH split search.
	struct nSplit_SAH
	{
		int SPLIT_AXIS = 0;
		int SPLIT_INDEX = 0;
		float SPLIT_COST = 0.0f;
	};

	// Base class for static and dynamic bounding-volume hierarchies.
	template<typename Entity>
	class nAABBTree
	{
	public:
		// -- Constructors --
		nAABBTree() = default;
		nAABBTree(const nAABBTree&) = delete;
		nAABBTree& operator=(const nAABBTree&) = delete;
		nAABBTree(nAABBTree&&) noexcept = default;
		nAABBTree& operator=(nAABBTree&&) & noexcept = default;
		virtual ~nAABBTree() = default;

		// -- Build / update interface --
		void BuildAABBTree(std::vector<Entity*>& entities);
		virtual void UpdateEntity(nBVHNode<Entity>* leaf) { (void)leaf; }
		virtual void RemoveEntity(nBVHNode<Entity>* leaf) { (void)leaf; }
		virtual void TreeletStepRestructure() {}
		virtual void Rebuild(std::vector<Entity*>& entities) = 0;
		virtual void InsertEntity(Entity* entity) { (void)entity; }
		virtual void Clear() = 0;

		// -- Traversal / queries --
		void DebugDrawTree(std::function<void(const nAABB&, int, bool, bool)> callback);
		nBVHNode<Entity>* GetRoot() { return root.get(); }

		static void TraverseCrossOverlaps(std::vector<std::pair<Entity*, Entity*>>& overlapEntities,
			const nBVHNode<Entity>* firstTree, const nBVHNode<Entity>* secTree);

		static void TraverseOverlaps(std::vector<std::pair<Entity*, Entity*>>& overlapEntities,
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
		nSplit_SAH bestSplit{};
		bestSplit.SPLIT_COST = FLT_MAX;
		bestSplit.SPLIT_AXIS = 0;

		// Parent surface area normalizes the cost.
		const nAABB parentAABB = nBVHTraits<Node>::ComputeBounds(entities, start, end);
		float parentAABB_SA = CalSurfaceArea(parentAABB);
		if (parentAABB_SA <= 1e-6f) parentAABB_SA = 1e-6f;
		const int count = end - start;
		bestSplit.SPLIT_INDEX = start + count / 2;

		for (int axis = 0; axis < 3; ++axis)
		{
			std::sort(entities.begin() + start,
				entities.begin() + end,
				[axis](const Node* a, const Node* b)
				{
					return Centroid(nBVHTraits<Node>::GetAABB(*a), axis) <
						Centroid(nBVHTraits<Node>::GetAABB(*b), axis);
				});

			// Prefix / suffix sweeps of merged bounds and surface areas.
			std::vector<nAABB> leftAABB(count);
			std::vector<float> leftSA(count);

			leftAABB[0] = nBVHTraits<Node>::GetAABB(*entities[start]);
			leftSA[0] = CalSurfaceArea(leftAABB[0]);
			for (int i = 1; i < count; ++i)
			{
				leftAABB[i] = Merge(leftAABB[i - 1], nBVHTraits<Node>::GetAABB(*entities[start + i]));
				leftSA[i] = CalSurfaceArea(leftAABB[i]);
			}

			std::vector<nAABB> rightAABB(count);
			std::vector<float> rightSA(count);

			rightAABB[count - 1] = nBVHTraits<Node>::GetAABB(*entities[end - 1]);
			rightSA[count - 1] = CalSurfaceArea(rightAABB[count - 1]);
			for (int i = count - 2; i >= 0; --i)
			{
				rightAABB[i] = Merge(rightAABB[i + 1], nBVHTraits<Node>::GetAABB(*entities[start + i]));
				rightSA[i] = CalSurfaceArea(rightAABB[i]);
			}

			for (int i = 0; i < count - 1; ++i)
			{
				const int leftCount = i + 1;
				const int rightCount = count - leftCount;
				const float cost = (leftCount * leftSA[i] +
					rightCount * rightSA[i + 1]) / parentAABB_SA;

				if (cost < bestSplit.SPLIT_COST)
				{
					bestSplit.SPLIT_COST = cost;
					bestSplit.SPLIT_AXIS = axis;
					bestSplit.SPLIT_INDEX = start + leftCount;
				}
			}
		}

		return bestSplit;
	}

	template<typename Entity>
	template<typename Node>
	std::unique_ptr<nBVHNode<Entity>> nAABBTree<Entity>::ConstructSAH(std::vector<Node*>& entities, int start, int end)
	{
		// Single element: leaf node.
		if (end - start == 1)
		{
			return nAABBTreeTraits<Node>::CreateLeafNode(entities[start]);
		}

		const auto [axis, split, cost] = FindBestSplitSAH(entities, start, end);
		(void)cost;

		// Re-sort by centroid along the chosen axis (FindBestSplitSAH may have
		// been computed on a different axis ordering).
		std::sort(entities.begin() + start, entities.begin() + end,
			[axis](const Node* a, const Node* b)
			{
				return Centroid(nBVHTraits<Node>::GetAABB(*a), axis) < Centroid(nBVHTraits<Node>::GetAABB(*b), axis);
			});

		std::unique_ptr<nBVHNode<Entity>> node = std::make_unique<nBVHNode<Entity>>();

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
		root = ConstructSAH<Entity>(entities, 0, static_cast<int>(entities.size()));
	}

	template<typename Entity>
	void nAABBTree<Entity>::DebugDrawTreeNode(nBVHNode<Entity>* node, int depth, std::function<void(const nAABB&, int, bool, bool)> callback)
	{
		if (!node) return;
		const bool leaf = node->isLeaf();
		callback(node->nodeAABB, depth, leaf, node->isRefit);
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
		s.avgDepth = static_cast<float>(depth);

		if (node->isRefit)    s.dirtyNodes = 1;
		if (node->isLeaf()) { s.leafNodes = 1; return s; }

		const nBVHStats left = CollectStatsRecursive(node->leftChild.get(), depth + 1);
		const nBVHStats right = CollectStatsRecursive(node->rightChild.get(), depth + 1);

		s.totalNodes += left.totalNodes + right.totalNodes;
		s.leafNodes += left.leafNodes + right.leafNodes;
		s.dirtyNodes += left.dirtyNodes + right.dirtyNodes;
		s.maxDepth = std::max({ s.maxDepth, left.maxDepth, right.maxDepth });
		s.avgDepth = (s.avgDepth + left.avgDepth + right.avgDepth) / 3.0f;

		return s;
	}

	template<typename Entity>
	void nAABBTree<Entity>::TraverseOverlaps(std::vector<std::pair<Entity*, Entity*>>& overlapEntities,
		const nBVHNode<Entity>* a, const nBVHNode<Entity>* b)
	{
		if (!a || !b) return;
		if (!Overlaps(a->nodeAABB, b->nodeAABB))
			return;

		if (a->isLeaf() && b->isLeaf())
		{
			// Skip self-pairs; report each pair once (ordered by ID).
			if (a->CollEntity != b->CollEntity)
			{
				if (nBVHTraits<Entity>::GetID(*a->CollEntity) < nBVHTraits<Entity>::GetID(*b->CollEntity))
				{
					overlapEntities.push_back({ a->CollEntity, b->CollEntity });
				}
			}
			return;
		}

		if (!a->isLeaf() && !b->isLeaf())
		{
			TraverseOverlaps(overlapEntities, a->leftChild.get(), b->leftChild.get());
			TraverseOverlaps(overlapEntities, a->rightChild.get(), b->rightChild.get());
			TraverseOverlaps(overlapEntities, a->leftChild.get(), b->rightChild.get());
			TraverseOverlaps(overlapEntities, a->rightChild.get(), b->leftChild.get());
		}
		else if (a->isLeaf())
		{
			TraverseOverlaps(overlapEntities, a, b->leftChild.get());
			TraverseOverlaps(overlapEntities, a, b->rightChild.get());
		}
		else
		{
			TraverseOverlaps(overlapEntities, a->leftChild.get(), b);
			TraverseOverlaps(overlapEntities, a->rightChild.get(), b);
		}
	}

	template<typename Entity>
	void nAABBTree<Entity>::TraverseCrossOverlaps(std::vector<std::pair<Entity*, Entity*>>& overlapEntities,
		const nBVHNode<Entity>* firstTree, const nBVHNode<Entity>* secTree)
	{
		if (!firstTree || !secTree) return;
		if (!Overlaps(firstTree->nodeAABB, secTree->nodeAABB))
			return;

		if (firstTree->isLeaf() && secTree->isLeaf())
		{
			overlapEntities.push_back({ firstTree->CollEntity, secTree->CollEntity });
			return;
		}

		if (!firstTree->isLeaf() && !secTree->isLeaf())
		{
			TraverseCrossOverlaps(overlapEntities, firstTree->leftChild.get(), secTree->leftChild.get());
			TraverseCrossOverlaps(overlapEntities, firstTree->rightChild.get(), secTree->rightChild.get());
			TraverseCrossOverlaps(overlapEntities, firstTree->leftChild.get(), secTree->rightChild.get());
			TraverseCrossOverlaps(overlapEntities, firstTree->rightChild.get(), secTree->leftChild.get());
		}
		else if (firstTree->isLeaf())
		{
			TraverseCrossOverlaps(overlapEntities, firstTree, secTree->leftChild.get());
			TraverseCrossOverlaps(overlapEntities, firstTree, secTree->rightChild.get());
		}
		else
		{
			TraverseCrossOverlaps(overlapEntities, firstTree->leftChild.get(), secTree);
			TraverseCrossOverlaps(overlapEntities, firstTree->rightChild.get(), secTree);
		}
	}
}
