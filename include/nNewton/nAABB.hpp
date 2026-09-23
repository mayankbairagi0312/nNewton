#pragma once
#include "nMath.hpp"

namespace nNewton
{
	// Axis-aligned bounding box.
	struct nAABB
	{
		nVector3 min;
		nVector3 max;

		// -- Constructors --
		constexpr nAABB() noexcept = default;
		constexpr nAABB(const nVector3& min_, const nVector3& max_) noexcept
			: min(min_), max(max_)
		{
		}
		constexpr nAABB(const nAABB&) noexcept = default;
		constexpr nAABB(nAABB&&) noexcept = default;
		constexpr nAABB& operator=(const nAABB&) & noexcept = default;
		constexpr nAABB& operator=(nAABB&&) & noexcept = default;
		~nAABB() = default;

		// -- Queries --
		nVector3 Center() const noexcept { return (min + max) * 0.5f; }
		nVector3 HalfExtents() const noexcept { return (max - min) * 0.5f; }
		float SurfaceArea() const noexcept
		{
			const nVector3 e = max - min; // extents
			return 2.0f * (e.x * e.y + e.y * e.z + e.z * e.x);
		}
	};

	inline float CalSurfaceArea(const nAABB& aabb) noexcept
	{
		const nVector3 ext = aabb.max - aabb.min;
		return 2.0f * (ext.x * ext.y + ext.y * ext.z + ext.z * ext.x);
	}

	inline float Centroid(const nAABB& a, int axis = 0) noexcept
	{
		const nVector3 center = a.Center();
		if (axis == 0) return center.x;
		if (axis == 1) return center.y;
		return center.z;
	}

	inline nAABB Expand(const nAABB& a, float margin) noexcept
	{
		return nAABB(
			nVector3(a.min.x - margin, a.min.y - margin, a.min.z - margin),
			nVector3(a.max.x + margin, a.max.y + margin, a.max.z + margin)
		);
	}

	inline nAABB Merge(const nAABB& a, const nAABB& b) noexcept
	{
		return nAABB(Min(a.min, b.min), Max(a.max, b.max));
	}

	inline bool Contains(const nAABB& outer, const nAABB& inner) noexcept
	{
		return outer.min.x <= inner.min.x &&
			outer.min.y <= inner.min.y &&
			outer.min.z <= inner.min.z &&
			outer.max.x >= inner.max.x &&
			outer.max.y >= inner.max.y &&
			outer.max.z >= inner.max.z;
	}

	inline bool Overlaps(const nAABB& a, const nAABB& b) noexcept
	{
		return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
			(a.min.y <= b.max.y && a.max.y >= b.min.y) &&
			(a.min.z <= b.max.z && a.max.z >= b.min.z);
	}
}
