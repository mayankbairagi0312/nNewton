#pragma once

#include <cstdint>
#include "nTypes.hpp"

namespace nNewton {

enum class nCollisionShapeType { nBox, nSphere, nCapsule, nConvex };

struct nCollisionShape
{
    nCollisionShapeType type = nCollisionShapeType::nBox;
    nCollider_ID ColliderID = INVALID_COLLIDER_ID;
};


template<class ShapeType>
struct nCollider
{
    uint16_t refCount = 0;
    ShapeType Collider{};
};

} // namespace nNewton