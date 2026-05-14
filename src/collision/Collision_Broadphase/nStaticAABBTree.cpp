#include <nNewton/nAABBTree.hpp>
#include<iostream>
namespace nNewton
{
    class nStaticAABBTree : public nAABBTree
    {
    public:
        void Rebuild(std::vector<nCollisionEntity*>& entities) override
        {
            std::cout << "Tree rbuild func called " << std::endl;
            Clear();
            BuildAABBTree(entities);
        }

        void Clear() override
        {
            root.reset(); 
        }
        
    private:
        
    };
}
