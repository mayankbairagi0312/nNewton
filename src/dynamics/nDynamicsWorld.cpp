#include <nNewton/nDynamicsWorld.hpp>

namespace nNewton
{

	
	nEntity_ID nDynamicsWorld::Create_Entity(const nRigidBodyInfo& info_)
	{
		nEntity_ID index_;

		if (!m_FreeList.empty())
		{
			index_ = m_FreeList.back();
			m_FreeList.pop_back();
		}
		else
		{
			index_ = (uint32_t)m_Entity.size();
			m_Entity.emplace_back();
		}
		
		auto& entity = m_Entity[index_];
		entity.alive = true;
		entity.Entity = nRigidBody(info_);
		return MAKE_ID(index_, entity.Gen);
	}

	void nDynamicsWorld::DestroyEntity(nEntity_ID id)
	{	
		auto index = INDEX_FROM_ID(id);
		auto gen = GEN_FROM_ID(id);

		if (index >= m_Entity.size())
			return;
		auto& e = m_Entity[index];

		if (!e.alive || e.Gen != gen)
			return;

		e.alive = false;
		e.Entity = nRigidBody();
		e.Gen++;
		m_FreeList.push_back(index);
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
		
	}
}