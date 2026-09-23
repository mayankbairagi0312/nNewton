#pragma once

#include <cstdint>
#include <concepts>

namespace nNewton {

using nEntity_ID = uint32_t;

constexpr nEntity_ID INVALID_ENTITY = 0u;

//20 bits index | 12 bits generation.
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
