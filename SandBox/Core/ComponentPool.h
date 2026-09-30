#pragma once

#include <cstdint>
#include <vector>
#include <utility>
#include <nNewton/nTypes.hpp>
#include <nNewton/nSlotAllocator.hpp>

struct ComponentSlot
{
    uint32_t handle = nNewton::INVALID_SLOT_HANDLE;
};

class IComponentPool
{
public:
    virtual ~IComponentPool() = default;
    virtual size_t Size() const = 0;
    virtual nNewton::nEntity_ID OwnerAt(uint32_t index) const = 0;
    virtual nNewton::nEntity_ID Remove(uint32_t index) = 0;
};

template<typename T>
class ComponentPool final : public IComponentPool
{
public:
    struct Entry { nNewton::nEntity_ID ownerID; T component; };

    // -- Constructors --
    ComponentPool() = default;
    ComponentPool(const ComponentPool&) = delete;
    ComponentPool& operator=(const ComponentPool&) = delete;
    ComponentPool(ComponentPool&&) noexcept = default;
    ComponentPool& operator=(ComponentPool&&) & noexcept = default;
    ~ComponentPool() override = default;

    // -- Component management --
    uint32_t Add(nNewton::nEntity_ID owner, const T& component)
    {
        return m_alloc.emplace(Entry{owner, component});
    }

    T& Get(uint32_t handle)
    {
        Entry* entry = m_alloc.get(handle);
        assert(entry && "Invalid component handle");
        return entry->component;
    }

    nNewton::nEntity_ID Remove(uint32_t handle) override
    {
        Entry* entry = m_alloc.get(handle);
        if (!entry) return nNewton::INVALID_ENTITY;

        nNewton::nEntity_ID moved = nNewton::INVALID_ENTITY;
        m_alloc.release(handle);
        return moved;
    }

    // -- Queries --
    size_t Size() const override { return m_alloc.aliveCount(); }
    nNewton::nEntity_ID OwnerAt(uint32_t handle) const override
    {
        const Entry* entry = m_alloc.get(handle);
        return entry ? entry->ownerID : nNewton::INVALID_ENTITY;
    }

    // -- Iteration --
    template<typename Fn>
    void ForEach(Fn&& fn)
    {
        m_alloc.forEach([&](Entry& entry) { fn(entry.ownerID, entry.component); });
    }

    template<typename Fn>
    void ForEach(Fn&& fn) const
    {
        m_alloc.forEach([&](const Entry& entry) { fn(entry.ownerID, entry.component); });
    }

private:
    nNewton::nSlotAllocator<Entry> m_alloc;
};