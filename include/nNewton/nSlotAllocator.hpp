#pragma once

#include <cstdint>
#include <vector>
#include <utility>
#include <cassert>
#include <algorithm>

#include "nTypes.hpp"

namespace nNewton {

// ---------------------------------------------------------------------------
// SoA design (Structure of Arrays):
//   m_values   — hot data, one flat array of T
//   m_gens     — 4B per slot, generation counters
//   m_alive    — 1B per slot, alive flags
//   m_freeList — freed indices (never index 0)
// Emplace => writes into pre-allocated slot memory; no placement new.
// ---------------------------------------------------------------------------
template<typename T>
class nSlotAllocator
{
public:
    // --- Ctor / dtor ---
    nSlotAllocator() = default;
    nSlotAllocator(const nSlotAllocator&) = delete;
    nSlotAllocator& operator=(const nSlotAllocator&) = delete;
    nSlotAllocator(nSlotAllocator&& other) noexcept
        : m_values(std::move(other.m_values)),
          m_gens(std::move(other.m_gens)),
          m_alive(std::move(other.m_alive)),
          m_freeList(std::move(other.m_freeList)),
          m_aliveCount(other.m_aliveCount)
    {
        other.m_aliveCount = 0;
    }
    nSlotAllocator& operator=(nSlotAllocator&& other) noexcept
    {
        if (this != &other) {
            m_values      = std::move(other.m_values);
            m_gens        = std::move(other.m_gens);
            m_alive       = std::move(other.m_alive);
            m_freeList    = std::move(other.m_freeList);
            m_aliveCount  = other.m_aliveCount;
            other.m_aliveCount = 0;
        }
        return *this;
    }
    ~nSlotAllocator() = default;

    // --- Capacity ---
    [[nodiscard]] size_t   size() const noexcept      { return m_values.size(); }
    [[nodiscard]] size_t   capacity() const noexcept  { return m_values.capacity(); }
    [[nodiscard]] size_t   freeCount() const noexcept { return m_freeList.size(); }
    [[nodiscard]] size_t   aliveCount() const noexcept { return m_aliveCount; }
    [[nodiscard]] bool     empty() const noexcept     { return m_aliveCount == 0; }

    void reserve(size_t n) { m_values.reserve(n); m_gens.reserve(n); m_alive.reserve(n); }

    // --- Allocation ---

    template<typename... Args>
    nSlotHandle emplace(Args&&... args)
    {
        uint32_t index;
        if (!m_freeList.empty())
        {
            index = m_freeList.back();
            m_freeList.pop_back();
            assert(index != 0); // index 0 is never in the free list
            // Recycled slot: MUST overwrite with the new value. The old AoS
            // code placement-new'd here; skipping this write leaves stale
            // data from the previous owner (e.g. an old component's state).
            m_values[index] = T(std::forward<Args>(args)...);
        }
        else
        {
            index = static_cast<uint32_t>(m_values.size());
            if (index == 0)
            {
                // First allocation on a fresh allocator: grow to slot 1 so
                // slot 0 exists as a permanently dead sentinel.
                m_values.emplace_back();
                m_gens.push_back(0);
                m_alive.push_back(0);
                index = 1;
            }
            m_values.emplace_back(std::forward<Args>(args)...);
            m_gens.push_back(0);   // gen starts at 0; first handle has gen 0
            m_alive.push_back(0);
        }

        assert(!m_alive[index]);
        m_alive[index] = 1;
        ++m_aliveCount;
        return SLOT_MAKE_HANDLE(index, m_gens[index]);
    }

    nSlotHandle insert(const T& v)  { return emplace(v); }
    nSlotHandle insert(T&& v)       { return emplace(std::move(v)); }

    // --- Access ---
    T* get(nSlotHandle handle) noexcept
    {
        if (!SLOT_VALID(handle)) return nullptr;
        const uint32_t idx = SLOT_INDEX(handle);
        if (idx >= m_values.size()) return nullptr;
        if (!m_alive[idx] || m_gens[idx] != SLOT_GEN(handle)) return nullptr;
        return &m_values[idx];
    }

    const T* get(nSlotHandle handle) const noexcept
    {
        if (!SLOT_VALID(handle)) return nullptr;
        const uint32_t idx = SLOT_INDEX(handle);
        if (idx >= m_values.size()) return nullptr;
        if (!m_alive[idx] || m_gens[idx] != SLOT_GEN(handle)) return nullptr;
        return &m_values[idx];
    }

    T* getByIndex(uint32_t index) noexcept
    {
        if (index >= m_values.size()) return nullptr;
        if (!m_alive[index]) return nullptr;
        return &m_values[index];
    }

    const T* getByIndex(uint32_t index) const noexcept
    {
        if (index >= m_values.size()) return nullptr;
        if (!m_alive[index]) return nullptr;
        return &m_values[index];
    }

    // Unchecked access
    T* getUnsafe(nSlotHandle handle) noexcept
    {
        assert(SLOT_VALID(handle));
        const uint32_t idx = SLOT_INDEX(handle);
        assert(idx < m_values.size());
        assert(m_alive[idx]);
        assert(m_gens[idx] == SLOT_GEN(handle));
        return &m_values[idx];
    }
    const T* getUnsafe(nSlotHandle handle) const noexcept
    {
        assert(SLOT_VALID(handle));
        const uint32_t idx = SLOT_INDEX(handle);
        assert(idx < m_values.size());
        assert(m_alive[idx]);
        assert(m_gens[idx] == SLOT_GEN(handle));
        return &m_values[idx];
    }

    // --- Release ---
    void release(nSlotHandle handle) noexcept
    {
        if (!SLOT_VALID(handle)) return;
        const uint32_t idx = SLOT_INDEX(handle);
        if (idx >= m_values.size()) return;
        if (!m_alive[idx] || m_gens[idx] != SLOT_GEN(handle)) return;

        // nSlotAllocator does not own its T's resources; no destructor call needed
        // because clear() / slot reuse simply marks the slot dead.
        m_alive[idx] = 0;
        --m_aliveCount;
        uint32_t g = ++m_gens[idx];
        if (g > SLOT_MAX_GEN) m_gens[idx] = 0; // wraparound
        m_freeList.push_back(idx);
    }

    // --- Iteration ---
    template<typename Fn>
    void forEach(Fn&& fn)
    {
        for (size_t i = 0; i < m_values.size(); ++i)
            if (m_alive[i])
                fn(m_values[i]);
    }

    template<typename Fn>
    void forEach(Fn&& fn) const
    {
        for (size_t i = 0; i < m_values.size(); ++i)
            if (m_alive[i])
                fn(m_values[i]);
    }

    template<typename Fn>
    void forEachWithHandle(Fn&& fn)
    {
        for (size_t i = 0; i < m_values.size(); ++i)
        {
            if (m_alive[i])
                fn(SLOT_MAKE_HANDLE(static_cast<uint32_t>(i), m_gens[i]), m_values[i]);
        }
    }

    template<typename Fn>
    void forEachWithHandle(Fn&& fn) const
    {
        for (size_t i = 0; i < m_values.size(); ++i)
        {
            if (m_alive[i])
                fn(SLOT_MAKE_HANDLE(static_cast<uint32_t>(i), m_gens[i]), m_values[i]);
        }
    }

    // -----
    void clear() noexcept
    {
        for (size_t i = 0; i < m_values.size(); ++i)
        {
            if (m_alive[i])
            {
                m_alive[i] = 0;
                uint32_t g = ++m_gens[i];
                if (g > SLOT_MAX_GEN) m_gens[i] = 0;
            }
        }
        m_aliveCount = 0;
        m_freeList.clear();
        m_freeList.reserve(m_values.size());
        // Index 0 is skipped: it never enters the free list.
        for (size_t i = 1; i < m_values.size(); ++i)
            m_freeList.push_back(static_cast<uint32_t>(i));
    }

    // --- Debug ---
    void debugPrint() const
    {
        size_t a = 0, d = 0;
        for (size_t i = 0; i < m_values.size(); ++i) { if (m_alive[i]) ++a; else ++d; }
        // printf("nSlotAllocator: alive=%zu dead=%zu freeList=%zu cap=%zu\n", a, d, m_freeList.size(), m_values.capacity());
    }

private:
    std::vector<T>         m_values;
    std::vector<uint32_t>  m_gens;
    std::vector<uint8_t>   m_alive;
    std::vector<uint32_t>  m_freeList;
    size_t                 m_aliveCount = 0;
};

} // namespace nNewton
