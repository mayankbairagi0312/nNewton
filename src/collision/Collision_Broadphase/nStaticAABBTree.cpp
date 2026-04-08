#include <nNewton/nAABBTree.hpp>

namespace nNewton
{
    class nStaticAABBTree : public nAABBTree
    {
    public:
        void Rebuild(std::vector<nCollisionEntity*>& entities) override
        {
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
