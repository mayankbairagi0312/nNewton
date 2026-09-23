#include <nNewton/nDynamicsWorld.hpp>
#include <cstdio>

namespace nNewton
{
	nDynamicsWorld::nDynamicsWorld()
		: m_CollisionWorld(std::make_unique<nCollisionWorld>())
	{
		// Index 0 is reserved as the invalid entity handle.
		m_Entity.emplace_back();
	}

	nEntity_ID nDynamicsWorld::Create_Entity(const nTransform& initTransform)
	{
		nEntity_ID index = 0;

		if (!m_FreeList.empty())
		{
			index = m_FreeList.back();
			m_FreeList.pop_back();
		}
		else
		{
			index = static_cast<nEntity_ID>(m_Entity.size());
			m_Entity.emplace_back();
		}

		nEntity& entity = m_Entity[index];
		entity.alive = true;
		entity.Entity = nRigidBody{};
		entity.Entity.TRANSFORM = initTransform;

		return MAKE_ID(index, entity.Gen);
	}

	nRigidBody& nDynamicsWorld::AddRigidBody(nEntity_ID id, const nRigidBodyInfo& info)
	{
		nEntity& slot = m_Entity[INDEX_FROM_ID(id)];
		assert(slot.alive && slot.Gen == GEN_FROM_ID(id));

		slot.Entity.TYPE = info.TYPE;
		slot.Entity.VELOCITY = info.INIT_VELOCITY;
		slot.Entity.DENSITY = info.DENSITY;
		slot.Entity.MASS_OVERRIDE = info.OVERRIDE_MASS ? info.MASS : 0.0f;

		if (info.TYPE == nBodyType::Dynamic)
		{
			RecomputeMassProperties(INDEX_FROM_ID(id));
		}

		return slot.Entity;
	}

	void nDynamicsWorld::RemoveRigidBody(nEntity_ID id)
	{
		nEntity& slot = m_Entity[INDEX_FROM_ID(id)];

		// Keep the transform alive across the reset so the entity can be re-added.
		if (!slot.Entity.ColEnt)
		{
			nTransform tf = slot.Entity.TRANSFORM;
			slot.Entity = nRigidBody();
			slot.Entity.TRANSFORM = tf;
			return;
		}

		nCollisionEntity* collider = slot.Entity.ColEnt;
		slot.Entity = nRigidBody();
		slot.Entity.TRANSFORM = collider->EntityTransform;
		slot.Entity.ColEnt = collider;
	}

	void nDynamicsWorld::RecomputeMassProperties(uint32_t idx)
	{
		nEntity& slot = m_Entity[idx];
		if (slot.Entity.TYPE != nBodyType::Dynamic || !slot.Entity.ColEnt || slot.Entity.ColEnt->EntityShape.ColliderID == INVALID_ENTITY)
		{
			slot.Entity.INV_MASS = 0.0f;
			return;
		}

		const bool isOverrideActive = (slot.Entity.MASS_OVERRIDE != 0.0f);
		float mass = 0.0f;

		if (isOverrideActive)
		{
			mass = slot.Entity.MASS_OVERRIDE;
		}
		else
		{
			nMassProperties mp = ComputeMassProperties(slot.Entity.ColEnt, slot.Entity.DENSITY);
			mass = mp.Mass;
		}

		slot.Entity.INV_MASS = mass > 0.0f ? 1.0f / mass : 0.0f;

		if (slot.Entity.ColEnt && slot.Entity.ColEnt->EntityShape.ColliderID != INVALID_ENTITY)
		{
			slot.Entity.INERTIA_TENSOR_INV_LOCAL =
				(GetUnitInertia(slot.Entity.ColEnt->EntityShape, m_CollisionWorld->GetColliderPool()) * mass).Inverse();
		}
		else
		{
			slot.Entity.INERTIA_TENSOR_INV_LOCAL = nMatrix3();
		}
	}

	void nDynamicsWorld::DestroyAllEntity()
	{
		m_CollisionWorld->RemoveAll();
		m_Entity.clear();
		m_FreeList.clear();
	}

	nRigidBody* nDynamicsWorld::GetBody(nEntity_ID id)
	{
		const uint32_t index = INDEX_FROM_ID(id);
		const uint32_t gen = GEN_FROM_ID(id);

		if (index >= m_Entity.size())
			return nullptr;

		nEntity& e = m_Entity[index];
		if (!e.alive || e.Gen != gen)
			return nullptr;

		return &e.Entity;
	}

	const nRigidBody* nDynamicsWorld::GetBody(nEntity_ID id) const
	{
		const uint32_t index = INDEX_FROM_ID(id);
		const uint32_t gen = GEN_FROM_ID(id);

		if (index >= m_Entity.size())
			return nullptr;

		const nEntity& e = m_Entity[index];
		if (!e.alive || e.Gen != gen)
			return nullptr;

		return &e.Entity;
	}

	const nTransform* nDynamicsWorld::GetTransform(nEntity_ID id) const
	{
		const uint32_t index = INDEX_FROM_ID(id);
		const uint32_t gen = GEN_FROM_ID(id);

		if (index >= m_Entity.size())
			return nullptr;

		const nEntity& e = m_Entity[index];
		if (!e.alive || e.Gen != gen)
			return nullptr;

		return &e.Entity.TRANSFORM;
	}

	bool nDynamicsWorld::IsValid(nEntity_ID id) const
	{
		const uint32_t index = INDEX_FROM_ID(id);
		const uint32_t gen = GEN_FROM_ID(id);

		if (index >= m_Entity.size())
			return false;

		const nEntity& e = m_Entity[index];
		return e.alive && e.Gen == gen;
	}

	void nDynamicsWorld::Step(float deltaT)
	{
		for (nEntity& entity : m_Entity)
		{
			if (!entity.alive) continue;
			if (entity.Entity.IsStatic()) continue;

			nRigidBody& body = entity.Entity;

			// Gravity is proportional to the overridden mass (0 when not overridden).
			body.ApplyForce(GetGravity() * body.MASS_OVERRIDE);

			body.Integrate(deltaT);
			body.ClearForces();

			// Sync the collision proxy transform with the simulated body.
			if (entity.Entity.ColEnt)
			{
				entity.Entity.ColEnt->EntityTransform = body.TRANSFORM;
			}
		}

		m_CollisionWorld->StepCollision();
	}
}
