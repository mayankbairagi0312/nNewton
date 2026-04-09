#pragma once

#include"nCollisionTypes.hpp"
#include "nDynamicsWorld.hpp"
#include "nAABBTree.hpp"


namespace nNewton
{
	
	class nCollisionWorld
	{
	public:
		nCollisionWorld();

		bool INIT_COLLISION_WORLD();

		nCollisionEntity* CreateCollisionEntity(nEntity_ID& ID, bool isStatic, const nTransform& EntityTransform, const nVector3& vel);
		bool RemoveCollisionEntity(nEntity_ID& ID, bool isStatic);
		void StepCollision(nCollisionEntity* entity);
		void BuildTrees();
		
		
	private:
		std::vector<std::unique_ptr<nCollisionEntity>> m_Static_Entities;
		std::vector<std::unique_ptr<nCollisionEntity>> m_Dynamic_Entities;

		std::unique_ptr<nAABBTree> m_DynamicTree;
		std::unique_ptr<nAABBTree> m_StaticTree;
		
		template<typename T>
		std::vector<T*> ToRawPtrs(std::vector<std::unique_ptr<T>>& entities)
		{
			std::vector<T*> raw;
			raw.reserve(entities.size());
			for (auto& e : entities)
				raw.push_back(e.get());
			return raw;
		}
	};

	

}