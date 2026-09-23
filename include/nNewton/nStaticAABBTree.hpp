#pragma once

#include "nAABBTree.hpp"

namespace nNewton
{
	template<class Entity>
	class nStaticAABBTree : public nAABBTree<Entity>
	{
	public:
		// -- Constructors --
		nStaticAABBTree() noexcept = default;
		nStaticAABBTree(const nStaticAABBTree&) = delete;
		nStaticAABBTree& operator=(const nStaticAABBTree&) = delete;
		nStaticAABBTree(nStaticAABBTree&&) noexcept = default;
		nStaticAABBTree& operator=(nStaticAABBTree&&) & noexcept = default;
		~nStaticAABBTree() override = default;

		// -- Build / update interface --
		void Rebuild(std::vector<Entity*>& entities) override
		{
			Clear();
			nAABBTree<Entity>::BuildAABBTree(entities);
		}

		void Clear() override
		{
			nAABBTree<Entity>::root.reset();
		}
	};
}
