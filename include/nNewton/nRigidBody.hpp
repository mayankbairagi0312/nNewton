#pragma once 

#include <iostream>
#include "nTransform.hpp"
#include <memory>   
#include"nCollisionShapes.hpp"
#include "nBoxShape.hpp"
#include "nCollisionTypes.hpp"
#include"nSphereShape.hpp"

namespace nNewton {
	struct nRigidBodyInfo {
		float MASS_ = 1.0f;
		nVector3 INIT_VELOCITY_;
		nTransform INIT_TRANSFORM_;
		bool IS_STATIC_ = false;
		nCollisionEntity* ColEnt = nullptr;

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
			std::cout << m_Shape << "\n";
			return m_Shape;
		}

	};
	struct nRigidBody
	{
		float MASS_ = 1.0f;
		nVector3 VELOCITY_;
		nVector3 FORCE_ACC_ = {};
		nTransform TRANSFORM_;
		bool IS_STATIC_ = false;

		nVector3 ANGULAR_VELOCITY_ = {};
		//nVector3 ANGULAR_VELOCITY_ = {};
		nVector3 TORQUE_ACC_ = {};
		nMatrix3 INERTIA_TENSOR_INV_ = {};  

		nCollisionEntity* ColEnt = nullptr;

		void ApplyForce(nVector3 Force);
		void SetVelocity(nVector3);


		nVector3 GetForce()const;
		float GetMass()const { return MASS_; }
		nVector3 GetVelocity()const { return VELOCITY_; }

		void ClearForces();

		void Integrate(float dt_);

		nRigidBody();
		explicit nRigidBody(const nRigidBodyInfo& Info_) : MASS_(Info_.MASS_), VELOCITY_(Info_.INIT_VELOCITY_), TRANSFORM_(Info_.INIT_TRANSFORM_), IS_STATIC_(Info_.IS_STATIC_)
		{ }
	};
}