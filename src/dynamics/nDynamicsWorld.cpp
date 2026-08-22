#include <nNewton/nDynamicsWorld.hpp>

namespace nNewton
{
	nDynamicsWorld::nDynamicsWorld() {
		m_CollisionWorld = std::make_unique<nCollisionWorld>();
		m_Entity.emplace_back();
	}
	
	nEntity_ID nDynamicsWorld::Create_Entity(const nTransform& InitTransform)
	{
		nEntity_ID index_;

		if (!m_FreeList.empty()){
			index_ = m_FreeList.back();
			m_FreeList.pop_back();
		}else{
			index_ = (uint32_t)m_Entity.size();
			m_Entity.emplace_back();
		}
		
		auto& entity = m_Entity[index_];
		entity.alive = true;
		entity.Entity = nRigidBody{};
		entity.Entity.TRANSFORM_ = InitTransform;

		return MAKE_ID(index_, entity.Gen);
	}
	
	nRigidBody& nDynamicsWorld::AddRigidBody(nEntity_ID id, const nRigidBodyInfo& info) {
		auto& slot = m_Entity[INDEX_FROM_ID(id)];
		assert(slot.alive && slot.Gen == GEN_FROM_ID(id));

		slot.Entity.TYPE_ = info.TYPE_;
		slot.Entity.VELOCITY_ = info.INIT_VELOCITY_;
		slot.Entity.DENSITY_ = info.DENSITY_;
		slot.Entity.MASS_OVERRIDE_ = info.OVERRIDE_MASS_ ? info.MASS_ : 0.0f;

		if (info.TYPE_ == nBodyType::Dynamic)
		{
			RecomputeMassProperties(INDEX_FROM_ID(id));
		}

		return slot.Entity;
	}
	void nDynamicsWorld::RemoveRigidBody(nEntity_ID id)
	{
		auto& slot = m_Entity[INDEX_FROM_ID(id)];

		if (!slot.Entity.ColEnt) { 
			nTransform tf = std::move(slot.Entity.TRANSFORM_);
			slot.Entity = nRigidBody();
			slot.Entity.TRANSFORM_ = tf;
			printf("Remove func Scale: %.2f %.2f %.2f\n", slot.Entity.TRANSFORM_.GetScale().x, slot.Entity.TRANSFORM_.GetScale().y, slot.Entity.TRANSFORM_.GetScale().z);

			return;
		}
		auto* collider = slot.Entity.ColEnt;
		slot.Entity = nRigidBody();
		slot.Entity.TRANSFORM_ = collider->EntityTransform;
		printf("Remove func Scale: %.2f %.2f %.2f\n", slot.Entity.TRANSFORM_.GetScale().x, slot.Entity.TRANSFORM_.GetScale().y, slot.Entity.TRANSFORM_.GetScale().z);

		slot.Entity.ColEnt = collider;
	}



	void nDynamicsWorld::RecomputeMassProperties(uint32_t idx) {

		auto& slot = m_Entity[idx];
		if (slot.Entity.TYPE_ != nBodyType::Dynamic || !slot.Entity.ColEnt || slot.Entity.ColEnt->EntityShape.ColliderID == INVALID_ENTITY)
		{
			slot.Entity.INV_MASS_ = 0.0f;
			return;
		}

		bool isOverrideActive = (slot.Entity.MASS_OVERRIDE_ != 0.0f);
		float mass = 0.0f;

		if (isOverrideActive) {
			mass = slot.Entity.MASS_OVERRIDE_;          
		}
		else {
			
			if (!slot.Entity.ColEnt ||
				slot.Entity.ColEnt->EntityShape.ColliderID == INVALID_ENTITY) {
				slot.Entity.INV_MASS_ = 0.0f;        
				return;
			}
			nMassProperties mp = ComputeMassProperties(slot.Entity.ColEnt, slot.Entity.DENSITY_);
			mass = mp.Mass;
		}


		slot.Entity.INV_MASS_ = mass > 0.0f ? 1.0f / mass : 0.0f;

		if (slot.Entity.ColEnt &&
			slot.Entity.ColEnt->EntityShape.ColliderID != INVALID_ENTITY) {
			slot.Entity.INERTIA_TENSOR_INV_LOCAL_ =
				(GetUnitInertia(slot.Entity.ColEnt->EntityShape, m_CollisionWorld->GetColliderPool()) * mass).Inverse();
		}
		else {
			slot.Entity.INERTIA_TENSOR_INV_LOCAL_ = nMatrix3();
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
		auto index = INDEX_FROM_ID(id);
		auto gen = GEN_FROM_ID(id);

		if (index >= m_Entity.size())
			return nullptr;

		auto& e = m_Entity[index];

		if (!e.alive || e.Gen != gen)
			return nullptr;
		
		return &e.Entity;
	}

	const nRigidBody* nDynamicsWorld::GetBody(nEntity_ID id) const
	{
		auto index = INDEX_FROM_ID(id);
		auto gen = GEN_FROM_ID(id);

		if (index >= m_Entity.size())
			return nullptr;

		auto& e = m_Entity[index];

		if (!e.alive || e.Gen != gen)
			return nullptr;
		return &e.Entity;
	}

	const nTransform* nDynamicsWorld::GetTransform(nEntity_ID id) const
	{
		auto index = INDEX_FROM_ID(id);
		auto gen = GEN_FROM_ID(id);

		if (index >= m_Entity.size())
			return nullptr;

		auto& e = m_Entity[index];

		if (!e.alive || e.Gen != gen)
			return nullptr;


		return &e.Entity.TRANSFORM_;
	}

	bool nDynamicsWorld::IsValid(nEntity_ID id_) const {
		auto index = INDEX_FROM_ID(id_);
		auto gen = GEN_FROM_ID(id_);

		if (index >= m_Entity.size())
			return false;

		const auto& e = m_Entity[index];

		if (!e.alive || e.Gen != gen)
			return false;

		return true;
	}

	void nDynamicsWorld::Step(float deltaT_)
	{
		for (auto& entity : m_Entity)
		{
			if (!entity.alive)         continue;
			if (entity.Entity.IsStatic()) continue;

			nRigidBody& body = entity.Entity;
			body.ApplyForce(GetGravity() * body.MASS_OVERRIDE_);

			body.Integrate(deltaT_);
			
			body.ClearForces();

			if (entity.Entity.ColEnt) {
				entity.Entity.ColEnt->EntityTransform = body.TRANSFORM_;
			}
		
		}

		m_CollisionWorld->StepCollision();
	}
}