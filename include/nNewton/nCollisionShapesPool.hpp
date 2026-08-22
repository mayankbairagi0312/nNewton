#pragma once

#include <vector>


#include "nBoxShape.hpp"
#include "nSphereShape.hpp"


namespace nNewton {

	class nCollisionShapePool {
	public:
		template<class ShapeType>
		nCollisionShape createCollider(ShapeType collider) {

			auto& vec = Shapes<ShapeType>();
			auto& free = FreeList<ShapeType>();

			uint32_t idx;
			if (!free.empty()) { idx = free.back(); free.pop_back(); }
			else { idx = (uint32_t)vec.size(); vec.push_back({}); }

			auto& slot = vec[idx];
			slot.Collider = collider;
			slot.refCount = 1;
			slot.alive = true;

			return { ShapeType::nTYPE,MAKE_ID(idx ,slot.gen) };
		}

		template<class ShapeType>
		ShapeType* getCollider(nCollisionShape h)
		{
			auto& slot = Shapes<ShapeType>()[INDEX_FROM_ID(h.ColliderID)];
			if (!slot.alive || slot.gen != GEN_FROM_ID(h.ColliderID))
				return nullptr;
			return &slot.Collider;
		}

		template<class ShapeType>
		void addRef(nCollisionShape h)
		{
			auto& slot = Shapes<ShapeType>()[INDEX_FROM_ID(h.ColliderID)];
			if (!slot.alive || slot.gen != GEN_FROM_ID(h.ColliderID))
				return;
			slot.refCount++;
		}

		template<class ShapeType>
		void removeCollider(nCollisionShape h)
		{
			auto& slot = Shapes<ShapeType>()[INDEX_FROM_ID(h.ColliderID)];
			if (slot.gen != GEN_FROM_ID(h.ColliderID)) return;

			if (--slot.refCount == 0)
			{
				slot.alive = false;
				slot.gen++;
				FreeList<ShapeType>().push_back(INDEX_FROM_ID(h.ColliderID));
			}
		}


	private:
		template<class ShapeType> std::vector<nCollider<ShapeType>>& Shapes();
		template<class ShapeType> std::vector<uint32_t>& FreeList();

		std::vector<nCollider<nBoxShape>>      m_Box;
		std::vector<nCollider<nSphereShape>>   m_Sphere;
		std::vector<uint32_t> m_BoxFree, m_SphereFree;
	};

	template<> inline std::vector<nCollider<nBoxShape>>& nCollisionShapePool::Shapes<nBoxShape>() { return m_Box; }
	template<> inline std::vector<nCollider<nSphereShape>>& nCollisionShapePool::Shapes<nSphereShape>() { return m_Sphere; }
	template<> inline std::vector<uint32_t>& nCollisionShapePool::FreeList<nBoxShape>() { return m_BoxFree; }
	template<> inline std::vector<uint32_t>& nCollisionShapePool::FreeList<nSphereShape>() { return m_SphereFree; }


	inline nAABB GetWorldAABB(const nCollisionShape& handle, const nTransform& transform,
		nCollisionShapePool& pool) {
		switch (handle.type) {
		case nCollisionShapeType::nBox: {
			auto* box = pool.getCollider<nBoxShape>(handle);
			if (box) return box->getAABB(transform);
			break;
		}
		case nCollisionShapeType::nSphere: {
			auto* sphere = pool.getCollider<nSphereShape>(handle);
			if (sphere) return sphere->getAABB(transform);
			break;
		}

		}
		return nAABB();
	}

	inline float GetVolume(const nCollisionShape& handle,
		nCollisionShapePool& pool) {
		switch (handle.type) {
		case nCollisionShapeType::nBox: {
			auto* box = pool.getCollider<nBoxShape>(handle);
			if (box) return box->getVolume();
			break;
		}
		case nCollisionShapeType::nSphere: {
			auto* sphere = pool.getCollider<nSphereShape>(handle);
			if (sphere) return sphere->getVolume();
			break;
		}

		}
		return 0;
	}

	inline nVector3 GetCentroid(const nCollisionShape& handle,
		nCollisionShapePool& pool) {
		switch (handle.type) {
		case nCollisionShapeType::nBox: {
			auto* box = pool.getCollider<nBoxShape>(handle);
			if (box) return box->getCentroid();
			break;
		}
		case nCollisionShapeType::nSphere: {
			auto* sphere = pool.getCollider<nSphereShape>(handle);
			if (sphere) return sphere->getCentroid();
			break;
		}

		}
		return nVector3();
	}

	inline nMatrix3 GetUnitInertia(const nCollisionShape& handle,
		nCollisionShapePool& pool) {
		switch (handle.type) {
		case nCollisionShapeType::nBox: {
			auto* box = pool.getCollider<nBoxShape>(handle);
			if (box) return box->getUnitInertia();
			break;
		}
		case nCollisionShapeType::nSphere: {
			auto* sphere = pool.getCollider<nSphereShape>(handle);
			if (sphere) return sphere->getUnitInertia();
			break;
		}

		}
		return nMatrix3();
	}
}