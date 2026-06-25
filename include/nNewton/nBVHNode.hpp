#pragma once

#include "nCollisionTypes.hpp"
#include "nCollisionShapes.hpp"
#include <memory>

namespace nNewton {

	template<typename Entity>
	struct nBVHNode
	{
		nAABB nodeAABB;
		std::unique_ptr <nBVHNode<Entity>> leftChild;
		std::unique_ptr <nBVHNode<Entity>> rightChild;
		nBVHNode<Entity>* parent;

		Entity* CollEntity = nullptr;

		bool inQueue = false;
		nBVHNode<Entity>* qPrev = nullptr;
		nBVHNode<Entity>* qNext = nullptr;

		bool isRefit = false;

		bool isLeaf() const { return CollEntity != nullptr; }
		nBVHNode() : leftChild(nullptr), rightChild(nullptr), CollEntity(nullptr), parent(nullptr)
		{
		}

	};

}