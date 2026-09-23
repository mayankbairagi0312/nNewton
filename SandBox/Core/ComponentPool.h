#pragma once
#include <cstdint>
#include <vector>
#include <utility>
#include <nNewton/nTypes.hpp>

struct ComponentSlot
{
    uint32_t index = UINT32_MAX;
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
        m_data.push_back({ owner, component });
        return static_cast<uint32_t>(m_data.size()) - 1;
    }

    T& Get(uint32_t index) { return m_data[index].component; }

    nNewton::nEntity_ID Remove(uint32_t index) override
    {
        const uint32_t last = static_cast<uint32_t>(m_data.size()) - 1;
        nNewton::nEntity_ID moved = nNewton::INVALID_ENTITY;
        if (index != last)
        {
            moved = m_data[last].ownerID;
            m_data[index] = std::move(m_data[last]);
        }
        m_data.pop_back();
        return moved;
    }

    // -- Queries --
    size_t Size() const override { return m_data.size(); }
    nNewton::nEntity_ID OwnerAt(uint32_t index) const override { return m_data[index].ownerID; }

    // -- Iteration --
    auto begin() { return m_data.begin(); }
    auto end() { return m_data.end(); }

private:
    std::vector<Entry> m_data;
};
