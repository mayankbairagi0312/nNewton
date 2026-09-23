#pragma once

#include "nCollisionTypes.hpp"
#include "nCollisionShapes.hpp"
#include "nAABB.hpp"
#include <memory>

namespace nNewton {

	// Node of a bounding-volume hierarchy. Owned children (unique_ptr) and
	// a raw parent pointer; leaf nodes point back to their entity.
	template<typename Entity>
	struct nBVHNode
	{
		nAABB nodeAABB;
		std::unique_ptr<nBVHNode<Entity>> leftChild = nullptr;
		std::unique_ptr<nBVHNode<Entity>> rightChild = nullptr;
		nBVHNode<Entity>* parent = nullptr;

		Entity* CollEntity = nullptr;

		// Intrusive queue links (dynamic tree refit queue)
		bool inQueue = false;
		nBVHNode<Entity>* qPrev = nullptr;
		nBVHNode<Entity>* qNext = nullptr;

		bool isRefit = false;

		// -- Constructors --
		nBVHNode() noexcept = default;
		nBVHNode(const nBVHNode&) = delete;
		nBVHNode& operator=(const nBVHNode&) = delete;
		nBVHNode(nBVHNode&&) noexcept = default;
		nBVHNode& operator=(nBVHNode&&) & noexcept = default;
		~nBVHNode() = default;

		// -- Queries --
		bool isLeaf() const noexcept { return CollEntity != nullptr; }
	};

}
