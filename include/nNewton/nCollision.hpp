#pragma once

#include"nCollisionTypes.hpp"
#include "nDynamicsWorld.hpp"
#include "nAABBTree.hpp"


namespace nNewton
{
	
	class nCollisionWorld
	{
	public:
		nCollisionShape* CreateCollisionEntity(nEntity_ID& ID, bool isStatic, const nTransform& EntityTransform, const nVector3& vel) 
		{

			nCollisionEntity data;
			data.EntityID = ID;
			data.isStatic = isStatic;
			data.EntityTransform = EntityTransform;
			data.vel = vel;

			auto ent = std::make_unique<nCollisionEntity>(std::move(data));
			nCollisionEntity* ptr = ent.get();

			if (ent->isStatic)
			{
				m_Static_Entities.push_back(std::move(ent));
			}
			else
			{
				m_Dynamic_Entities.push_back(std::move(ent));
			}
			
			return ptr;

		}

		
		void BuildTrees()
		{
			/*m_StaticTree.BuildAABBTree(m_Static_Entities);
			m_DynamicTree.BuildAABBTree(m_Dynamic_Entities);*/
		}

	private:
		std::vector<std::unique_ptr<nCollisionEntity>> m_Static_Entities;
		std::vector<std::unique_ptr<nCollisionEntity>> m_Dynamic_Entities;
		/*nAABBTree m_StaticTree;
		nAABBTree m_DynamicTree;*/
		
	};

	

}