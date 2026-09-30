#pragma once

#include <cstdint>

namespace nNewton {

enum class nCollisionShapeType { nBox, nSphere, nCapsule, nConvex };

struct nCollisionShape
{
    nCollisionShapeType type = nCollisionShapeType::nBox;
    uint32_t ColliderID = INVALID_COLLIDER_ID;

    static constexpr uint32_t INVALID_COLLIDER_ID = UINT32_MAX;
};


template<class ShapeType>
struct nCollider
{
    uint16_t refCount = 0;
    ShapeType Collider{};
};

} // namespace nNewton