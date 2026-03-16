#include <nNewton/nAABBTree.hpp>

namespace nNewton
{
    class nStaticBVHTree : public nAABBTree
    {
    public:
        
        nBVHNode* GetRoot() const override { return root.get(); }

        
    private:
        std::unique_ptr<nBVHNode> root;
    };
}