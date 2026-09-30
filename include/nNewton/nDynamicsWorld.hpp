#pragma once

#include <cassert>
#include <cstdint>
#include <memory>
#include "nTransform.hpp"
#include "nRigidBody.hpp"
#include "nCollision.hpp"
#include "nTypes.hpp"
#include "nSlotAllocator.hpp"

namespace nNewton {

struct nEntity
{
    // -- Constructors --
    nEntity() = default;
    nEntity(const nEntity&) = default;
    nEntity(nEntity&&) noexcept = default;
    nEntity& operator=(const nEntity&) & = default;
    nEntity& operator=(nEntity&&) & noexcept = default;
    ~nEntity() = default;

    nRigidBody Entity{};
};

class nDynamicsWorld
{
public:
    // -- Constructors --
    nDynamicsWorld();
    nDynamicsWorld(const nDynamicsWorld&) = delete;
    nDynamicsWorld& operator=(const nDynamicsWorld&) = delete;
    nDynamicsWorld(nDynamicsWorld&&) noexcept = default;
    nDynamicsWorld& operator=(nDynamicsWorld&&) & noexcept = default;
    ~nDynamicsWorld() = default;

    // -- Entity management --
    nEntity_ID Create_Entity(const nTransform& initTransform);

    template<class ShapeType>
    void DestroyEntity(nEntity_ID id);

    void DestroyAllEntity();

    // -- Bodies --
    nRigidBody& AddRigidBody(nEntity_ID id, const nRigidBodyInfo& info);
    void RemoveRigidBody(nEntity_ID id);

    template<class ShapeType>
    nCollisionEntity* AddCollider(nEntity_ID id, ShapeType&& shape,
        const nTransform& localXf, bool insertNow);

    template<class ShapeType>
    void RemoveCollider(nEntity_ID id);

    template <class T>
    nMassProperties ComputeMassProperties(const T*, float) = delete;

    void RecomputeMassProperties(uint32_t idx);

    // -- Queries --
    nRigidBody* GetBody(nEntity_ID id);
    const nRigidBody* GetBody(nEntity_ID id) const;

    const nTransform* GetTransform(nEntity_ID id) const;
    bool IsValid(nEntity_ID id) const;

    nCollisionShape* GetColliderShape(nEntity_ID id) const
    {
        if (!IsValid(id)) return nullptr;
        const nRigidBody* body = GetBody(id);
        if (!body || !body->ColEnt) return nullptr;
        return &body->ColEnt->EntityShape;
    }

    // -- Simulation --
    void Step(float deltaT);

    // -- Accessors --
    nCollisionWorld* GetCollisionWorld() { return m_CollisionWorld.get(); }
    const nCollisionWorld* GetCollisionWorld() const { return m_CollisionWorld.get(); }

    void SetGravity(nVector3 gravity) noexcept { m_Gravity = gravity; }
    nVector3 GetGravity() const noexcept { return m_Gravity; }

private:
    nSlotAllocator<nEntity> m_entityAlloc;
    std::unique_ptr<nCollisionWorld> m_CollisionWorld;

    nVector3 m_Gravity = { 0.0f, 0.0f, 0.0f };
};


template<class ShapeType>
inline nCollisionEntity* nDynamicsWorld::AddCollider(nEntity_ID id, ShapeType&& shape,
    const nTransform& localXf, bool insertNow)
{
    nEntity* slot = m_entityAlloc.get(id);
    assert(slot);

    nCollisionEntity* collider = m_CollisionWorld->CreateCollisionEntity(
        id, slot->Entity.TYPE, localXf, slot->Entity.GetVelocity(),
        std::forward<ShapeType>(shape), insertNow);

    slot->Entity.ColEnt = collider;
    return collider;
}

template<class ShapeType>
inline void nDynamicsWorld::RemoveCollider(nEntity_ID id)
{
    if (!IsValid(id)) return;
    nEntity* slot = m_entityAlloc.get(id);
    if (!slot) return;

    m_CollisionWorld->RemoveCollisionEntity<ShapeType>(id, slot->Entity.IsStatic());
    slot->Entity.ColEnt = nullptr;
}

template<class ShapeType>
inline void nDynamicsWorld::DestroyEntity(nEntity_ID id)
{
    const uint32_t index = INDEX_FROM_ID(id);
    const uint32_t gen = GEN_FROM_ID(id);

    nEntity* slot = m_entityAlloc.get(id);
    if (!slot) return;

    m_CollisionWorld->RemoveCollisionEntity<ShapeType>(id, slot->Entity.IsStatic());

    m_entityAlloc.release(id);
}

template<>
inline nMassProperties nDynamicsWorld::ComputeMassProperties<nCollisionEntity>(
    const nCollisionEntity* collShape, float density)
{
    nMassProperties out{};
    if (!collShape) return out;

    const float mass = GetVolume(collShape->EntityShape, m_CollisionWorld->GetColliderPool()) * density;
    const nVector3 com = collShape->EntityTransform.TransformPt(
        GetCentroid(collShape->EntityShape, m_CollisionWorld->GetColliderPool()));

    out.Mass = mass;
    out.CenterOfMass = com;

    if (out.Mass <= 0.0f) return out;

    out.Inertia = GetUnitInertia(collShape->EntityShape, m_CollisionWorld->GetColliderPool()) * mass;
    return out;
}

} // namespace nNewton