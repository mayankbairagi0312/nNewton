#include <nNewton/nDynamicsWorld.hpp>
#include <cstdio>

namespace nNewton
{
    nDynamicsWorld::nDynamicsWorld()
        : m_CollisionWorld(std::make_unique<nCollisionWorld>())
    {
    }

    nEntity_ID nDynamicsWorld::Create_Entity(const nTransform& initTransform)
    {
        nEntity_ID handle = m_entityAlloc.emplace(nEntity{});
        nEntity* entity = m_entityAlloc.getUnsafe(handle);
        
        entity->Entity = nRigidBody{};
        entity->Entity.TRANSFORM = initTransform;

        return handle;
    }

    nRigidBody& nDynamicsWorld::AddRigidBody(nEntity_ID id, const nRigidBodyInfo& info)
    {
        nEntity* slot = m_entityAlloc.get(id);
        assert(slot);

        slot->Entity.TYPE = info.TYPE;
        slot->Entity.VELOCITY = info.INIT_VELOCITY;
        slot->Entity.DENSITY = info.DENSITY;
        slot->Entity.MASS_OVERRIDE = info.OVERRIDE_MASS ? info.MASS : 0.0f;

        if (info.TYPE == nBodyType::Dynamic)
        {
            RecomputeMassProperties(INDEX_FROM_ID(id));
        }

        return slot->Entity;
    }

    void nDynamicsWorld::RemoveRigidBody(nEntity_ID id)
    {
        nEntity* slot = m_entityAlloc.get(id);
        if (!slot) return;

        
        if (!slot->Entity.ColEnt)
        {
            nTransform tf = slot->Entity.TRANSFORM;
            slot->Entity = nRigidBody();
            slot->Entity.TRANSFORM = tf;
            return;
        }

        nCollisionEntity* collider = slot->Entity.ColEnt;
        slot->Entity = nRigidBody();
        slot->Entity.TRANSFORM = collider->EntityTransform;
        slot->Entity.ColEnt = collider;
    }

    void nDynamicsWorld::RecomputeMassProperties(uint32_t idx)
    {
        if (idx >= m_entityAlloc.size()) return;

        nEntity* slot = m_entityAlloc.getByIndex(idx);
        if (!slot) return;

        const bool hasCollider = slot->Entity.ColEnt
            && SLOT_VALID(slot->Entity.ColEnt->EntityShape.ColliderID);

        if (slot->Entity.TYPE != nBodyType::Dynamic || !hasCollider)
        {
            slot->Entity.INV_MASS = 0.0f;
            return;
        }

        const bool isOverrideActive = (slot->Entity.MASS_OVERRIDE != 0.0f);
        float mass = 0.0f;

        if (isOverrideActive)
        {
            mass = slot->Entity.MASS_OVERRIDE;
        }
        else
        {
            nMassProperties mp = ComputeMassProperties(slot->Entity.ColEnt, slot->Entity.DENSITY);
            mass = mp.Mass;
        }

        slot->Entity.INV_MASS = mass > 0.0f ? 1.0f / mass : 0.0f;

        if (hasCollider)
        {
            slot->Entity.INERTIA_TENSOR_INV_LOCAL =
                (GetUnitInertia(slot->Entity.ColEnt->EntityShape, m_CollisionWorld->GetColliderPool()) * mass).Inverse();
        }
        else
        {
            slot->Entity.INERTIA_TENSOR_INV_LOCAL = nMatrix3();
        }
    }

    void nDynamicsWorld::DestroyAllEntity()
    {
        m_CollisionWorld->RemoveAll();
        m_entityAlloc.clear();
    }

    nRigidBody* nDynamicsWorld::GetBody(nEntity_ID id)
    {
        nEntity* slot = m_entityAlloc.get(id);
        return slot ? &slot->Entity : nullptr;
    }

    const nRigidBody* nDynamicsWorld::GetBody(nEntity_ID id) const
    {
        const nEntity* slot = m_entityAlloc.get(id);
        return slot ? &slot->Entity : nullptr;
    }

    const nTransform* nDynamicsWorld::GetTransform(nEntity_ID id) const
    {
        const nEntity* slot = m_entityAlloc.get(id);
        return slot ? &slot->Entity.TRANSFORM : nullptr;
    }

    bool nDynamicsWorld::IsValid(nEntity_ID id) const
    {
        const nEntity* slot = m_entityAlloc.get(id);
        return slot != nullptr;
    }

    void nDynamicsWorld::Step(float deltaT)
    {
        m_entityAlloc.forEach([this, deltaT](nEntity& entity) {
            if (entity.Entity.IsStatic()) return;

            nRigidBody& body = entity.Entity;

            const float invMass = body.GetInvMass();
            if (invMass > 0.0f)
                body.ApplyForce(GetGravity() * (1.0f / invMass));

            body.Integrate(deltaT);
            body.ClearForces();

            if (entity.Entity.ColEnt)
            {
                entity.Entity.ColEnt->EntityTransform = body.TRANSFORM;
            }
        });

        m_CollisionWorld->StepCollision();
    }
}