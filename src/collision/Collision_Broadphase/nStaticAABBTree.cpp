#include <nNewton/nAABBTree.hpp>

namespace nNewton
{
    class nStaticAABBTree : public nAABBTree
    {
    public:
        
        nBVHNode* GetRoot() const override { return root.get(); }

        
    private:
        std::unique_ptr<nBVHNode> root;
    };
}