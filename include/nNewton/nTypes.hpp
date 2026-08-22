#pragma once
#include<cstdint>

namespace nNewton
{
	using nEntity_ID = uint32_t;
	static constexpr nEntity_ID INVALID_ENTITY = 0;

	inline nEntity_ID MAKE_ID(uint32_t index_, uint32_t gen_)
	{
		return (gen_ << 20) | (index_ & 0xFFFFF);
	}

	inline uint32_t INDEX_FROM_ID(nEntity_ID id_)
	{
		return id_ & 0xFFFFF;
	}

	inline uint32_t GEN_FROM_ID(nEntity_ID id_)
	{
		return id_ >> 20;
	}
}