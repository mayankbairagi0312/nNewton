#pragma once

#include<cstdint>
#include<vector>
#include "nTransform.hpp"
#include "nRigidBody.hpp"
#include"nCollision.hpp"
#include "nTypes.hpp"

namespace nNewton {

	struct nEntity
	{
		nRigidBody Entity;
		uint32_t Gen = 0;
		bool      alive = false;
	};

	class nDynamicsWorld
	{	
	public:

		nDynamicsWorld();

		nEntity_ID Create_Entity(const nTransform& InitTransform);

		template<class ShapeType>
		void DestroyEntity(nEntity_ID id);

		void DestroyAllEntity();

		nRigidBody& AddRigidBody(nEntity_ID id, const nRigidBodyInfo& info);

		template<class ShapeType>
		nCollisionEntity* AddCollider(nEntity_ID id, ShapeType &&shape,
			const nTransform& localXf, bool insertNow);
		
		template<class ShapeType>
		void RemoveCollider(nEntity_ID id);
		void RemoveRigidBody(nEntity_ID id);

		template <class T>
		nMassProperties ComputeMassProperties(const T*, float) = delete;

		void RecomputeMassProperties(uint32_t idx);

		nRigidBody* GetBody(nEntity_ID id);
		const nRigidBody* GetBody(nEntity_ID id) const;

		const nTransform* GetTransform(nEntity_ID id) const;
		bool IsValid(nEntity_ID id_) const;

		void Step(float deltaT_);

		nCollisionWorld* GetCollisionWorld() { return m_CollisionWorld.get(); }
		const nCollisionWorld* GetCollisionWorld() const { return m_CollisionWorld.get(); }

		nCollisionShape* GetColliderShape(nEntity_ID id) const
		{
			if (!IsValid(id)) return nullptr;
			const nRigidBody* body = GetBody(id);
			if (!body || !body->ColEnt) return nullptr;
			return &body->ColEnt->EntityShape;
		}

		void SetGravity(nVector3 gravity) { m_Gravity = gravity; }
		nVector3 GetGravity()const { return m_Gravity; }
	private:
		std::vector<nEntity> m_Entity;
		std::vector<uint32_t> m_FreeList;
		std::unique_ptr<nCollisionWorld> m_CollisionWorld;

		nVector3 m_Gravity = { 0.0f, 0.0f, 0.0f };
	};


	template<class ShapeType>
	nCollisionEntity* nDynamicsWorld::AddCollider(nEntity_ID id, ShapeType&& shape,
		const nTransform& localXf, bool insertNow)
	{
		auto& slot = m_Entity[INDEX_FROM_ID(id)];
		assert(slot.alive && slot.Gen == GEN_FROM_ID(id));
		auto* h = m_CollisionWorld->CreateCollisionEntity(id, slot.Entity.TYPE_, localXf, slot.Entity.GetVelocity(), std::forward<ShapeType>(shape), insertNow);
		slot.Entity.ColEnt = h;

		return h;
	}


	template<class ShapeType>
	void nDynamicsWorld::RemoveCollider(nEntity_ID id)
	{
		if (!IsValid(id)) return;
		auto& slot = m_Entity[INDEX_FROM_ID(id)];
		m_CollisionWorld->RemoveCollisionEntity<ShapeType>(id, slot.Entity.IsStatic());
		slot.Entity.ColEnt = nullptr;
	}

	template<class ShapeType>
	void nDynamicsWorld::DestroyEntity(nEntity_ID id)
	{
		auto index = INDEX_FROM_ID(id);
		auto gen = GEN_FROM_ID(id);

		if (index >= m_Entity.size())
			return;
		auto& e = m_Entity[index];

		if (!e.alive || e.Gen != gen)
			return;

		m_CollisionWorld->RemoveCollisionEntity<ShapeType>(id, e.Entity.IsStatic());

		e.alive = false;
		e.Entity = nRigidBody();
		e.Gen++;
		m_FreeList.push_back(index);
	}



	template<>
	inline nMassProperties nDynamicsWorld::ComputeMassProperties<nCollisionEntity>(const nCollisionEntity* collShape, float density) {
		nMassProperties out{};
		if (!collShape) return out;

		const float    m = GetVolume(collShape->EntityShape,m_CollisionWorld->GetColliderPool() ) * density;
		const nVector3 com = collShape->EntityTransform.TransformPt(GetCentroid(collShape->EntityShape, m_CollisionWorld->GetColliderPool()));

		out.Mass = m;
		out.CenterOfMass = com;

		if (out.Mass <= 0.0f) return out;

		out.Inertia = GetUnitInertia(collShape->EntityShape, m_CollisionWorld->GetColliderPool()) * m;
		return out;
	}
}