#pragma once

#include <vector>

#include "nBoxShape.hpp"
#include "nSphereShape.hpp"

namespace nNewton {

	class nCollisionShapePool {
	public:
		// -- Constructors --
		nCollisionShapePool() = default;
		nCollisionShapePool(const nCollisionShapePool&) = delete;
		nCollisionShapePool& operator=(const nCollisionShapePool&) = delete;
		nCollisionShapePool(nCollisionShapePool&&) noexcept = default;
		nCollisionShapePool& operator=(nCollisionShapePool&&) & noexcept = default;
		~nCollisionShapePool() = default;

		// -- Shape management --
		template<class ShapeType>
		nCollisionShape createCollider(ShapeType collider)
		{
			std::vector<nCollider<ShapeType>>& vec = Shapes<ShapeType>();
			std::vector<uint32_t>& freeList = FreeList<ShapeType>();

			uint32_t idx;
			if (!freeList.empty())
			{
				idx = freeList.back();
				freeList.pop_back();
			}
			else
			{
				idx = static_cast<uint32_t>(vec.size());
				vec.push_back({});
			}

			nCollider<ShapeType>& slot = vec[idx];
			slot.Collider = std::move(collider);
			slot.refCount = 1;
			slot.alive = true;

			return { ShapeType::nTYPE, MAKE_ID(idx, slot.gen) };
		}

		template<class ShapeType>
		ShapeType* getCollider(nCollisionShape handle)
		{
			nCollider<ShapeType>& slot = Shapes<ShapeType>()[INDEX_FROM_ID(handle.ColliderID)];
			if (!slot.alive || slot.gen != GEN_FROM_ID(handle.ColliderID))
				return nullptr;
			return &slot.Collider;
		}

		template<class ShapeType>
		void addRef(nCollisionShape handle)
		{
			nCollider<ShapeType>& slot = Shapes<ShapeType>()[INDEX_FROM_ID(handle.ColliderID)];
			if (!slot.alive || slot.gen != GEN_FROM_ID(handle.ColliderID))
				return;
			++slot.refCount;
		}

		template<class ShapeType>
		void removeCollider(nCollisionShape handle)
		{
			nCollider<ShapeType>& slot = Shapes<ShapeType>()[INDEX_FROM_ID(handle.ColliderID)];
			if (slot.gen != GEN_FROM_ID(handle.ColliderID)) return;

			if (--slot.refCount == 0)
			{
				slot.alive = false;
				++slot.gen;
				FreeList<ShapeType>().push_back(INDEX_FROM_ID(handle.ColliderID));
			}
		}

	private:
		template<class ShapeType> std::vector<nCollider<ShapeType>>& Shapes();
		template<class ShapeType> std::vector<uint32_t>& FreeList();

		std::vector<nCollider<nBoxShape>> m_Box;
		std::vector<nCollider<nSphereShape>> m_Sphere;
		std::vector<uint32_t> m_BoxFree;
		std::vector<uint32_t> m_SphereFree;
	};

	template<> inline std::vector<nCollider<nBoxShape>>& nCollisionShapePool::Shapes<nBoxShape>() { return m_Box; }
	template<> inline std::vector<nCollider<nSphereShape>>& nCollisionShapePool::Shapes<nSphereShape>() { return m_Sphere; }
	template<> inline std::vector<uint32_t>& nCollisionShapePool::FreeList<nBoxShape>() { return m_BoxFree; }
	template<> inline std::vector<uint32_t>& nCollisionShapePool::FreeList<nSphereShape>() { return m_SphereFree; }

//====================== Shape dispatch helpers =======================//

	inline nAABB GetWorldAABB(const nCollisionShape& handle, const nTransform& transform,
		nCollisionShapePool& pool)
	{
		switch (handle.type) {
		case nCollisionShapeType::nBox: {
			if (auto* box = pool.getCollider<nBoxShape>(handle))
				return box->getAABB(transform);
			break;
		}
		case nCollisionShapeType::nSphere: {
			if (auto* sphere = pool.getCollider<nSphereShape>(handle))
				return sphere->getAABB(transform);
			break;
		}
		default: break;
		}
		return nAABB();
	}

	inline float GetVolume(const nCollisionShape& handle, nCollisionShapePool& pool)
	{
		switch (handle.type) {
		case nCollisionShapeType::nBox: {
			if (auto* box = pool.getCollider<nBoxShape>(handle))
				return box->getVolume();
			break;
		}
		case nCollisionShapeType::nSphere: {
			if (auto* sphere = pool.getCollider<nSphereShape>(handle))
				return sphere->getVolume();
			break;
		}
		default: break;
		}
		return 0.0f;
	}

	inline nVector3 GetCentroid(const nCollisionShape& handle, nCollisionShapePool& pool)
	{
		switch (handle.type) {
		case nCollisionShapeType::nBox: {
			if (auto* box = pool.getCollider<nBoxShape>(handle))
				return box->getCentroid();
			break;
		}
		case nCollisionShapeType::nSphere: {
			if (auto* sphere = pool.getCollider<nSphereShape>(handle))
				return sphere->getCentroid();
			break;
		}
		default: break;
		}
		return nVector3();
	}

	inline nMatrix3 GetUnitInertia(const nCollisionShape& handle, nCollisionShapePool& pool)
	{
		switch (handle.type) {
		case nCollisionShapeType::nBox: {
			if (auto* box = pool.getCollider<nBoxShape>(handle))
				return box->getUnitInertia();
			break;
		}
		case nCollisionShapeType::nSphere: {
			if (auto* sphere = pool.getCollider<nSphereShape>(handle))
				return sphere->getUnitInertia();
			break;
		}
		default: break;
		}
		return nMatrix3();
	}
}
