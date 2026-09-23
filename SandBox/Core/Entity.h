#pragma once
#include <nNewton/nTypes.hpp>
#include <nNewton/nDynamicsWorld.hpp>
#include "Component.h"
#include "ComponentPool.h"
#include <array>

struct eEntity
{
    nNewton::nEntity_ID eID = nNewton::INVALID_ENTITY;

    std::array<ComponentSlot, static_cast<size_t>(ComponentType::Count)> Slots;

    uint64_t Mask = 0;
};
