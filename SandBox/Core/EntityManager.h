#pragma once

#include "Entity.h"
#include "Component.h"
#include "ComponentPool.h"
#include <nNewton/nDynamicsWorld.hpp>
#include <array>
#include <cassert>
#include <vector>

class eManager
{
public:
    // -- Constructors --
    explicit eManager(nNewton::nDynamicsWorld& world) noexcept
        : m_world(world)
    {
    }
    eManager(const eManager&) = delete;
    eManager& operator=(const eManager&) = delete;
    eManager(eManager&&) noexcept = delete;
    eManager& operator=(eManager&&) noexcept = delete;
    ~eManager() = default;

    // -- Entity management --
    nNewton::nEntity_ID Spawn(const nNewton::nTransform& transform)
    {
        const nNewton::nEntity_ID id = m_world.Create_Entity(transform);

        const size_t idx = nNewton::INDEX_FROM_ID(id);
        if (idx >= entities.size()) entities.resize(idx + 1);
        entities[idx] = eEntity{};
        entities[idx].eID = id;
        return id;
    }

    template<class ShapeType>
    void Despawn(nNewton::nEntity_ID id)
    {
        eEntity& entity = entities[nNewton::INDEX_FROM_ID(id)];

        for (size_t t = 0; t < static_cast<size_t>(ComponentType::Count); ++t)
        {
            if (!(entity.Mask & (1ull << t))) continue;
            IComponentPool* pool = s_poolRegistry[t];
            const ComponentHandle handle = entity.Slots[t].handle;
            pool->Remove(handle);
        }
        entity.Mask = 0;
        m_world.DestroyEntity<ShapeType>(id);
    }

    bool IsAlive(nNewton::nEntity_ID id) const { return m_world.IsValid(id); }

    // -- Component management --
    template<typename T>
    T& AddComponent(nNewton::nEntity_ID id, const T& component)
    {
        assert(IsAlive(id) && "stale nEntity_ID");
        eEntity& entity = entities[nNewton::INDEX_FROM_ID(id)];
        ComponentPool<T>& pool = GetPool<T>();
        const ComponentHandle handle = pool.Add(id, component);
        constexpr auto type = ComponentTraits<T>::Value;
        entity.Slots[static_cast<size_t>(type)].handle = handle;
        entity.Mask |= (1ull << static_cast<size_t>(type));
        return pool.Get(handle);
    }

    template<typename T>
    void RemoveComponent(nNewton::nEntity_ID id)
    {
        constexpr auto type = ComponentTraits<T>::Value;
        eEntity& entity = entities[nNewton::INDEX_FROM_ID(id)];
        assert(entity.Mask & (1ull << static_cast<size_t>(type)));

        ComponentPool<T>& pool = GetPool<T>();
        const ComponentHandle handle = entity.Slots[static_cast<size_t>(type)].handle;
        pool.Remove(handle);

        entity.Slots[static_cast<size_t>(type)].handle = INVALID_COMPONENT_HANDLE;
        entity.Mask &= ~(1ull << static_cast<size_t>(type));
    }

    template<typename T>
    T* GetComponent(nNewton::nEntity_ID id)
    {
        eEntity& entity = entities[nNewton::INDEX_FROM_ID(id)];
        constexpr auto type = ComponentTraits<T>::Value;
        if (!(entity.Mask & (1ull << static_cast<size_t>(type)))) return nullptr;
        return &GetPool<T>().Get(entity.Slots[static_cast<size_t>(type)].handle);
    }

    template<typename T>
    bool HasComponent(nNewton::nEntity_ID id) const
    {
        constexpr auto type = ComponentTraits<T>::Value;
        return (entities[nNewton::INDEX_FROM_ID(id)].Mask & (1ull << static_cast<size_t>(type))) != 0;
    }

    template<typename T>
    ComponentPool<T>& GetPool()
    {
        static ComponentPool<T> pool;
        s_poolRegistry[static_cast<size_t>(ComponentTraits<T>::Value)] = &pool;
        return pool;
    }

    template<typename T, typename Fn>
    void ForEach(Fn&& fn)
    {
        GetPool<T>().ForEach(std::forward<Fn>(fn));
    }

    std::vector<eEntity> entities;

private:
    nNewton::nDynamicsWorld& m_world;
    static inline std::array<IComponentPool*, static_cast<size_t>(ComponentType::Count)> s_poolRegistry{};
};