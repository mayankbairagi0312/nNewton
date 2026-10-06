#pragma once

#include <cstdint>
#include <concepts>

namespace nNewton {

// ---------------------------------------------------------------------------
// Handle layout: 32 bits index | 32 bits generation (64-bit total).
// Index 0 is permanently dead; user's first allocated index is 1.
// INVALID_ENTITY = 0 (index=0, gen=0) can never be produced by emplace().
// ---------------------------------------------------------------------------
// Low-level slot allocator handle — any keyed pool.
using nSlotHandle = uint64_t;
constexpr nSlotHandle SLOT_MAX_HANDLE = 0xFFFFFFFFFFFFFFFFull;

// A collider handle inside the collision engine.
using nCollider_ID = nSlotHandle;
constexpr nCollider_ID INVALID_COLLIDER_ID = 0ull;

// An entity handle inside the engine's ECS.
using nEntity_ID = uint64_t;
constexpr nEntity_ID INVALID_ENTITY = 0ull;

constexpr uint32_t SLOT_MAX_INDEX = 0xFFFFFFFFu;  // 32 bits
constexpr uint32_t SLOT_MAX_GEN   = 0xFFFFFFFFu;  // 32 bits

constexpr nSlotHandle SLOT_MAKE_HANDLE(uint32_t index, uint32_t gen) noexcept
{
    return (static_cast<nSlotHandle>(gen) << 32) | static_cast<uint64_t>(index);
}

constexpr uint32_t SLOT_INDEX(nSlotHandle handle) noexcept
{
    return static_cast<uint32_t>(handle);
}

constexpr uint32_t SLOT_GEN(nSlotHandle handle) noexcept
{
    return static_cast<uint32_t>(handle >> 32);
}

constexpr bool SLOT_VALID(nSlotHandle handle) noexcept
{
    return handle != 0ull && handle != SLOT_MAX_HANDLE;
}

// Legacy helpers kept for API compatibility; same 32|32 layout.
constexpr uint32_t INDEX_FROM_ID(nEntity_ID id) noexcept { return SLOT_INDEX(id); }
constexpr uint32_t GEN_FROM_ID(nEntity_ID id)   noexcept { return SLOT_GEN(id); }
constexpr nSlotHandle MAKE_ID(uint32_t index, uint32_t gen) noexcept { return SLOT_MAKE_HANDLE(index, gen); }

// Type trait for entity IDs
template<typename T>
concept EntityID = std::same_as<T, nEntity_ID>;

} // namespace nNewton
