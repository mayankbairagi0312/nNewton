#pragma once

#include "nAABBTree.hpp"
#include<iostream>
namespace nNewton
{
    template<class Entity>
    class nStaticAABBTree : public nAABBTree<Entity>
    {
    public:
        void Rebuild(std::vector<Entity*>& entities) override
        {
            Clear();
            nAABBTree<Entity>::BuildAABBTree(entities);
        }

        void Clear() override
        {
            nAABBTree<Entity>::root.reset();
        }

    private:

    };
}
