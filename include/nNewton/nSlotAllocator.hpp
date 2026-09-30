#pragma once

#include <cstdint>
#include <vector>
#include <utility>
#include <cassert>
#include <algorithm>

#include "nTypes.hpp"

namespace nNewton {

template<typename T>
class nSlotAllocator
{
public:
    struct Slot
    {
        T          value{};
        uint32_t   gen = 0;
        bool       alive = false;
    };

    // --- Ctor / dtro ---
    nSlotAllocator() = default;
    nSlotAllocator(const nSlotAllocator&) = delete;
    nSlotAllocator& operator=(const nSlotAllocator&) = delete;
    nSlotAllocator(nSlotAllocator&& other) noexcept
        : m_slots(std::move(other.m_slots)), m_freeList(std::move(other.m_freeList)), m_aliveCount(other.m_aliveCount)
    {
        other.m_aliveCount = 0;
    }
    nSlotAllocator& operator=(nSlotAllocator&& other) noexcept
    {
        if (this != &other) {
            m_slots       = std::move(other.m_slots);
            m_freeList    = std::move(other.m_freeList);
            m_aliveCount  = other.m_aliveCount;
            other.m_aliveCount = 0;
        }
        return *this;
    }
    ~nSlotAllocator() = default;

    // --- Capacity ---
    [[nodiscard]] size_t size() const noexcept { return m_slots.size(); }
    [[nodiscard]] size_t capacity() const noexcept { return m_slots.capacity(); }
    [[nodiscard]] size_t freeCount() const noexcept { return m_freeList.size(); }
    [[nodiscard]] size_t aliveCount() const noexcept { return m_aliveCount; }
    [[nodiscard]] bool empty() const noexcept { return m_aliveCount == 0; }

    void reserve(size_t n) { m_slots.reserve(n); }

    // --- Allocation ---

    template<typename... Args>
    uint32_t emplace(Args&&... args)
    {
        uint32_t index;
        if (!m_freeList.empty())
        {
            index = m_freeList.back();
            m_freeList.pop_back();
        }
        else
        {
            index = static_cast<uint32_t>(m_slots.size());
            m_slots.emplace_back();
        }

        Slot& slot = m_slots[index];
        assert(!slot.alive);
        new (&slot.value) T(std::forward<Args>(args)...);
        slot.alive = true;
        ++m_aliveCount;
        return SLOT_MAKE_HANDLE(index, slot.gen);
    }

    uint32_t insert(const T& v)  { return emplace(v); }
    uint32_t insert(T&& v)       { return emplace(std::move(v)); }

    // --- Access ---
    T* get(uint32_t handle) noexcept
    {
        if (!SLOT_VALID(handle)) return nullptr;
        const uint32_t idx = SLOT_INDEX(handle);
        if (idx >= m_slots.size()) return nullptr;
        Slot& slot = m_slots[idx];
        if (!slot.alive || slot.gen != SLOT_GEN(handle)) return nullptr;
        return &slot.value;
    }

    const T* get(uint32_t handle) const noexcept
    {
        if (!SLOT_VALID(handle)) return nullptr;
        const uint32_t idx = SLOT_INDEX(handle);
        if (idx >= m_slots.size()) return nullptr;
        const Slot& slot = m_slots[idx];
        if (!slot.alive || slot.gen != SLOT_GEN(handle)) return nullptr;
        return &slot.value;
    }

    
    T* getByIndex(uint32_t index) noexcept
    {
        if (index >= m_slots.size()) return nullptr;
        Slot& slot = m_slots[index];
        if (!slot.alive) return nullptr;
        return &slot.value;
    }

    const T* getByIndex(uint32_t index) const noexcept
    {
        if (index >= m_slots.size()) return nullptr;
        const Slot& slot = m_slots[index];
        if (!slot.alive) return nullptr;
        return &slot.value;
    }

    // Unchecked access 
    T* getUnsafe(uint32_t handle) noexcept
    {
        assert(SLOT_VALID(handle));
        assert(SLOT_INDEX(handle) < m_slots.size());
        assert(m_slots[SLOT_INDEX(handle)].alive);
        assert(m_slots[SLOT_INDEX(handle)].gen == SLOT_GEN(handle));
        return &m_slots[SLOT_INDEX(handle)].value;
    }
    const T* getUnsafe(uint32_t handle) const noexcept
    {
        assert(SLOT_VALID(handle));
        assert(SLOT_INDEX(handle) < m_slots.size());
        assert(m_slots[SLOT_INDEX(handle)].alive);
        assert(m_slots[SLOT_INDEX(handle)].gen == SLOT_GEN(handle));
        return &m_slots[SLOT_INDEX(handle)].value;
    }

    // --- Release ---
    void release(uint32_t handle) noexcept
    {
        if (!SLOT_VALID(handle)) return;
        const uint32_t idx = SLOT_INDEX(handle);
        if (idx >= m_slots.size()) return;
        Slot& slot = m_slots[idx];
        if (!slot.alive || slot.gen != SLOT_GEN(handle)) return;

        slot.value.~T();
        slot.alive = false;
        --m_aliveCount;
        ++slot.gen;
        if (slot.gen > SLOT_MAX_GEN) slot.gen = 0; // wraparound 
        m_freeList.push_back(idx);
    }

    // --- Iteration ---
    template<typename Fn>
    void forEach(Fn&& fn)
    {
        for (Slot& slot : m_slots)
            if (slot.alive)
                fn(slot.value);
    }

    template<typename Fn>
    void forEach(Fn&& fn) const
    {
        for (const Slot& slot : m_slots)
            if (slot.alive)
                fn(slot.value);
    }

    template<typename Fn>
    void forEachWithHandle(Fn&& fn)
    {
        for (size_t i = 0; i < m_slots.size(); ++i)
        {
            Slot& slot = m_slots[i];
            if (slot.alive)
                fn(SLOT_MAKE_HANDLE(static_cast<uint32_t>(i), slot.gen), slot.value);
        }
    }

    template<typename Fn>
    void forEachWithHandle(Fn&& fn) const
    {
        for (size_t i = 0; i < m_slots.size(); ++i)
        {
            const Slot& slot = m_slots[i];
            if (slot.alive)
                fn(SLOT_MAKE_HANDLE(static_cast<uint32_t>(i), slot.gen), slot.value);
        }
    }

    // -----
    void clear() noexcept
    {
        for (Slot& slot : m_slots)
        {
            if (slot.alive)
            {
                slot.value.~T();
                slot.alive = false;
                ++slot.gen;
            }
        }
        m_aliveCount = 0;
        m_freeList.clear();
        m_freeList.reserve(m_slots.size());
        // Index 0 is reserved as invalid 
        for (size_t i = 1; i < m_slots.size(); ++i)
            m_freeList.push_back(static_cast<uint32_t>(i));
    }

    // --- Debug ---
    void debugPrint() const
    {
        size_t a = 0, d = 0;
        for (const auto& s : m_slots) { if (s.alive) ++a; else ++d; }
        // printf("nSlotAllocator: alive=%zu dead=%zu freeList=%zu cap=%zu\n", a, d, m_freeList.size(), m_slots.capacity());
    }

private:
    std::vector<Slot> m_slots;
    std::vector<uint32_t> m_freeList;
    size_t m_aliveCount = 0;
};

} // namespace nNewton