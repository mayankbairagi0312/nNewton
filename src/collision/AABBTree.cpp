#include <nNewton/nAABBTree.hpp>
#include <algorithm>

namespace nNewton
{
	void nAABBTree::BuildAABBTree(std::vector<nCollisionEntity*>& entities)
	{
		root = ConstructSAH(entities, 0, entities.size());
	}

	std::unique_ptr<nBVHNode> nAABBTree::ConstructSAH(std::vector<nCollisionEntity*>& entities, int start, int end)
	{
		if (end - start == 1)
		{
			return CreateLeafNode(entities[start]);
		}

		auto [axis, split, cost] = FindBestSplitSAH(entities, start, end);

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


	nSplit_SAH nAABBTree::FindBestSplitSAH(std::vector<nCollisionEntity*>& entities, int start, int end)
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
				leftAABB[i] = Merge(leftAABB[i - 1], entities[start + i]->currentAABB);
				leftSA[i] = CalSurfaceArea(leftAABB[i]);
			}


			std::vector<nAABB>  rightAABB(count);
			std::vector<float> rightSA(count);
			rightAABB[count - 1] = entities[end - 1]->currentAABB;
			for (int i = count - 2; i >= 0; i--)
			{
				rightAABB[i] = Merge(rightAABB[i + 1], entities[start + i]->currentAABB);
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


	std::unique_ptr<nBVHNode> nAABBTree::CreateLeafNode(nCollisionEntity* Ent_)
	{
		auto leaf = std::make_unique<nBVHNode>();
		leaf->Entity = Ent_;
		leaf->nodeAABB = Ent_->marginAABB;
		return leaf;
	}




}