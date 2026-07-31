#pragma once
#include <nNewton/nTypes.hpp>
#include <nNewton/nDynamicsWorld.hpp>
#include "Component.h"
#include "ComponentPool.h"
#include <array>

using namespace nNewton;

struct eEntity
{
    nNewton::nEntity_ID eID;

    std::array<ComponentSlot,
        (size_t)ComponentType::cCount> Slots;

    uint64_t Mask = 0;
};