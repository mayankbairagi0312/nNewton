#pragma once

#include <vector>
#include "nBoxShape.hpp"
#include "nSphereShape.hpp"
#include "nSlotAllocator.hpp"

namespace nNewton {

class nCollisionShapePool {
public:
    // -- Constructors --
    nCollisionShapePool() = default;
    nCollisionShapePool(const nCollisionShapePool&) = delete;
    nCollisionShapePool& operator=(const nCollisionShapePool&) = delete;
    nCollisionShapePool(nCollisionShapePool&&) noexcept = default;
    nCollisionShapePool& operator=(nCollisionShapePool&&) & noexcept = default;
    ~nCollisionShapePool() = default;

    // -- Shape management --
    template<class ShapeType>
    nCollisionShape createCollider(ShapeType collider)
    {
        nSlotAllocator<nCollider<ShapeType>>& alloc = Alloc<ShapeType>();
        
        nEntity_ID handle = alloc.emplace();
        nCollider<ShapeType>& slot = *alloc.getUnsafe(handle);
        
        slot.Collider = std::move(collider);
        slot.refCount = 1;

        return { ShapeType::nTYPE, handle };
    }

    template<class ShapeType>
    ShapeType* getCollider(nCollisionShape handle)
    {
        nSlotAllocator<nCollider<ShapeType>>& alloc = Alloc<ShapeType>();
        
        if (!nNewton::SLOT_VALID(handle.ColliderID)) return nullptr;
        
        nCollider<ShapeType>* slot = alloc.get(handle.ColliderID);
        if (!slot) return nullptr;
        return &slot->Collider;
    }

    template<class ShapeType>
    void addRef(nCollisionShape handle)
    {
        nSlotAllocator<nCollider<ShapeType>>& alloc = Alloc<ShapeType>();
        
        if (!nNewton::SLOT_VALID(handle.ColliderID)) return;
        
        nCollider<ShapeType>* slot = alloc.get(handle.ColliderID);
        if (slot) ++slot->refCount;
    }

    template<class ShapeType>
    void removeCollider(nCollisionShape handle)
    {
        nSlotAllocator<nCollider<ShapeType>>& alloc = Alloc<ShapeType>();
        
        if (!nNewton::SLOT_VALID(handle.ColliderID)) return;
        
        nCollider<ShapeType>* slot = alloc.get(handle.ColliderID);
        if (!slot) return;
        
        if (--slot->refCount == 0)
        {
            alloc.release(handle.ColliderID);
        }
    }

private:
    template<class ShapeType> nSlotAllocator<nCollider<ShapeType>>& Alloc();

    nSlotAllocator<nCollider<nBoxShape>>   m_boxAlloc;
    nSlotAllocator<nCollider<nSphereShape>> m_sphereAlloc;
};

template<> inline nSlotAllocator<nCollider<nBoxShape>>&   nCollisionShapePool::Alloc<nBoxShape>()   { return m_boxAlloc; }
template<> inline nSlotAllocator<nCollider<nSphereShape>>& nCollisionShapePool::Alloc<nSphereShape>() { return m_sphereAlloc; }

//====================== Shape dispatch helpers =======================//

inline nAABB GetWorldAABB(const nCollisionShape& handle, const nTransform& transform,
    nCollisionShapePool& pool)
{
    switch (handle.type) {
    case nCollisionShapeType::nBox: {
        if (auto* box = pool.getCollider<nBoxShape>(handle))
            return box->getAABB(transform);
        break;
    }
    case nCollisionShapeType::nSphere: {
        if (auto* sphere = pool.getCollider<nSphereShape>(handle))
            return sphere->getAABB(transform);
        break;
    }
    default: break;
    }
    return nAABB();
}

inline float GetVolume(const nCollisionShape& handle, nCollisionShapePool& pool)
{
    switch (handle.type) {
    case nCollisionShapeType::nBox: {
        if (auto* box = pool.getCollider<nBoxShape>(handle))
            return box->getVolume();
        break;
    }
    case nCollisionShapeType::nSphere: {
        if (auto* sphere = pool.getCollider<nSphereShape>(handle))
            return sphere->getVolume();
        break;
    }
    default: break;
    }
    return 0.0f;
}

inline nVector3 GetCentroid(const nCollisionShape& handle, nCollisionShapePool& pool)
{
    switch (handle.type) {
    case nCollisionShapeType::nBox: {
        if (auto* box = pool.getCollider<nBoxShape>(handle))
            return box->getCentroid();
        break;
    }
    case nCollisionShapeType::nSphere: {
        if (auto* sphere = pool.getCollider<nSphereShape>(handle))
            return sphere->getCentroid();
        break;
    }
    default: break;
    }
    return nVector3();
}

inline nMatrix3 GetUnitInertia(const nCollisionShape& handle, nCollisionShapePool& pool)
{
    switch (handle.type) {
    case nCollisionShapeType::nBox: {
        if (auto* box = pool.getCollider<nBoxShape>(handle))
            return box->getUnitInertia();
        break;
    }
    case nCollisionShapeType::nSphere: {
        if (auto* sphere = pool.getCollider<nSphereShape>(handle))
            return sphere->getUnitInertia();
        break;
    }
    default: break;
    }
    return nMatrix3();
}

} // namespace nNewton