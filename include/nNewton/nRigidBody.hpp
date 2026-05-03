#pragma once 

#include "nTransform.hpp"
#include <memory>   
#include"nCollisionShapes.hpp"
#include "nBoxShape.hpp"
#include"nSphereShape.hpp"

namespace nNewton {
	struct nRigidBodyInfo {
		float MASS_ = 1.0f;
		nVector3 INIT_VELOCITY_;
		nTransform INIT_TRANSFORM_;
		bool IS_STATIC_ = false;
	private:
		std::shared_ptr<nCollisionShape> m_Shape;

	public:
		void SetBoxShape(nVector3 hext)
		{
			m_Shape = std::make_shared<nNewton::nBoxShape>(hext);
		}

		void SetSphereShape(float Radi)
		{
			m_Shape = std::make_shared<nNewton::nSphereShape>(Radi);
		}

		std::shared_ptr<nCollisionShape> getCollisionShape() const
		{
			return m_Shape;
		}

	};
	struct nRigidBody
	{
		float MASS_ = 1.0f;
		nVector3 VELOCITY_;
		nTransform TRANSFORM_;
		bool IS_STATIC_ = false;

		nRigidBody();
		explicit nRigidBody(const nRigidBodyInfo& Info_) : MASS_(Info_.MASS_), VELOCITY_(Info_.INIT_VELOCITY_), TRANSFORM_(Info_.INIT_TRANSFORM_), IS_STATIC_(Info_.IS_STATIC_)
		{ }
	};
}