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

		nEntity_ID Create_Entity(const nRigidBodyInfo& _info, bool insertNow);

		void DestroyEntity(nEntity_ID id);

		nRigidBody* GetBody(nEntity_ID id);
		const nRigidBody* GetBody(nEntity_ID id) const;

		const nTransform* GetTransform(nEntity_ID id) const;
		bool IsValid(nEntity_ID id_) const;

		void Step(float deltaT_);

		nCollisionWorld* GetCollisionWorld() { return m_CollisionWorld.get(); }
		const nCollisionWorld* GetCollisionWorld() const { return m_CollisionWorld.get(); }


		void SetGravity(nVector3 gravity) { m_Gravity = gravity; }
		nVector3 GetGravity()const { return m_Gravity; }
	private:
		std::vector<nEntity> m_Entity;
		std::vector<uint32_t> m_FreeList;
		std::unique_ptr<nCollisionWorld> m_CollisionWorld;

		nVector3 m_Gravity = { 0.0f, 0.0f, 0.0f };
	};

}