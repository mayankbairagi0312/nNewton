#pragma once

#include<cstdint>
#include<vector>
#include "nTransform.hpp"
#include "nRigidBody.hpp"

namespace nNewton {


	using nEntity_ID = uint32_t;
	static constexpr nEntity_ID INVALID_ENTITY = 0;


	struct nEntity
	{
		nRigidBody Entity;
		uint32_t Gen = 0;
		bool      alive = false;
	};

	inline nEntity_ID MAKE_ID(uint32_t index_, uint32_t gen_)
	{
		return (gen_ << 16) | index_;
	}

	inline uint32_t INDEX_FROM_ID(nEntity_ID id_)
	{
		return id_ & 0xFFFF;
	}

	inline uint32_t GEN_FROM_ID(nEntity_ID id_)
	{
		return id_ >> 16;
	}

	class nDynamicsWorld
	{
	public:

		nEntity_ID Create_Entity(const nRigidBodyInfo& _info);

		void DestroyEntity(nEntity_ID id);

		nRigidBody* GetBody(nEntity_ID id);
		const nRigidBody* GetBody(nEntity_ID id) const;

		const nTransform* GetTransform(nEntity_ID id) const;
		bool IsValid(nEntity_ID id_) const;

	private:
		std::vector<nEntity> m_Entity;
		std::vector<uint32_t> m_FreeList;

		

	};

}