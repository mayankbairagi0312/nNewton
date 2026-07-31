
#pragma once
#include "Entity.h"
#include "Component.h"
#include "ComponentPool.h"
#include <nNewton/nDynamicsWorld.hpp>
#include <array>
#include <cassert>

class eManager
{
public:
    explicit eManager(nNewton::nDynamicsWorld& world) : m_world(world) {}

    nNewton::nEntity_ID Spawn( const nNewton::nTransform& Transform)
    {
        nNewton::nEntity_ID shapeID = m_world.Create_Entity(Transform);

        size_t idx = INDEX_FROM_ID(shapeID);
        if (idx >= entities.size()) entities.resize(idx + 1);
        entities[idx] = eEntity{};
        entities[idx].eID = shapeID;
        return shapeID;
    }
    template<class collider>
    void Despawn(nNewton::nEntity_ID id)
    {
        auto& e = entities[INDEX_FROM_ID(id)];
        for (size_t t = 0; t < (size_t)ComponentType::cCount; ++t)
        {
            if (!(e.Mask & (1ull << t))) continue;
            auto* pool = s_poolRegistry[t];
            uint32_t slot = e.Slots[t].index;
            auto moved = pool->Remove(slot);
            if (moved != nNewton::INVALID_ENTITY)
                entities[INDEX_FROM_ID(moved)].Slots[t].index = slot;
        }
        e.Mask = 0;
        m_world.DestroyEntity<collider>(id); 
    }

    bool IsAlive(nNewton::nEntity_ID id) const { return m_world.IsValid(id); }

    template<typename T>
    T& AddComponent(nNewton::nEntity_ID id, const T& c)
    {
        assert(IsAlive(id) && "stale nEntity_ID");
        auto& entity = entities[INDEX_FROM_ID(id)];
        auto& pool = GetPool<T>();
        uint32_t slot = pool.Add(id, c);
        constexpr auto type = ComponentTraits<T>::Value;
        entity.Slots[(size_t)type].index = slot;
        entity.Mask |= (1ull << (size_t)type);
        return pool.Get(slot);
    }

    template<typename T>
    void RemoveComponent(nNewton::nEntity_ID id)
    {
        constexpr auto type = ComponentTraits<T>::Value;
        auto& entity = entities[INDEX_FROM_ID(id)];
        assert(entity.Mask & (1ull << (size_t)type));

        auto& pool = GetPool<T>();
        uint32_t slot = entity.Slots[(size_t)type].index;
        auto moved = pool.Remove(slot);
        if (moved != nNewton::INVALID_ENTITY)
            entities[INDEX_FROM_ID(moved)].Slots[(size_t)type].index = slot;

        entity.Slots[(size_t)type].index = UINT32_MAX;
        entity.Mask &= ~(1ull << (size_t)type);
    }

    template<typename T>
    T* GetComponent(nNewton::nEntity_ID id)
    {
        auto& entity = entities[INDEX_FROM_ID(id)];
        constexpr auto type = ComponentTraits<T>::Value;
        if (!(entity.Mask & (1ull << (size_t)type))) return nullptr;
        return &GetPool<T>().Get(entity.Slots[(size_t)type].index);
    }

    template<typename T>
    bool HasComponent(nNewton::nEntity_ID id) const
    {
        constexpr auto type = ComponentTraits<T>::Value;
        return (entities[INDEX_FROM_ID(id)].Mask & (1ull << (size_t)type)) != 0;
    }

    template<typename T>
    ComponentPool<T>& GetPool()
    {
        static ComponentPool<T> pool;
        s_poolRegistry[(size_t)ComponentTraits<T>::Value] = &pool;
        return pool;
    }

    template<typename T, typename Fn>
    void ForEach(Fn&& fn)
    {
        for (auto& entry : GetPool<T>())
            fn(entry.owner, entry.component);
    }

    std::vector<eEntity> entities;

private:
    nNewton::nDynamicsWorld& m_world;
    static inline std::array<IComponentPool*, (size_t)ComponentType::cCount> s_poolRegistry{};
};