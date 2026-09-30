#pragma once

#include <cstdint>
#include <concepts>

namespace nNewton {

using nEntity_ID = uint32_t;

constexpr nEntity_ID INVALID_ENTITY = 0u;

//allocator constants
constexpr uint32_t SLOT_MAX_INDEX = 0xFFFFFu;
constexpr uint32_t SLOT_MAX_GEN   = 0xFFFFFu;
constexpr uint32_t INVALID_SLOT_HANDLE = 0xFFFFFFFFu;

constexpr uint32_t SLOT_MAKE_HANDLE(uint32_t index, uint32_t gen) noexcept
{
    return (gen << 20) | (index & SLOT_MAX_INDEX);
}

constexpr uint32_t SLOT_INDEX(uint32_t handle) noexcept
{
    return handle & SLOT_MAX_INDEX;
}

constexpr uint32_t SLOT_GEN(uint32_t handle) noexcept
{
    return handle >> 20;
}

constexpr bool SLOT_VALID(uint32_t handle) noexcept
{
    return handle != INVALID_SLOT_HANDLE;
}

// 20 bits index | 12 bits generation 
constexpr nEntity_ID MAKE_ID(uint32_t index, uint32_t gen) noexcept {
    return (gen << 20) | (index & 0xFFFFFu);
}

constexpr uint32_t INDEX_FROM_ID(nEntity_ID id) noexcept {
    return id & 0xFFFFFu;
}

constexpr uint32_t GEN_FROM_ID(nEntity_ID id) noexcept {
    return id >> 20;
}

// Type trait for entity IDs
template<typename T>
concept EntityID = std::same_as<T, nEntity_ID>;

} // namespace nNewton
