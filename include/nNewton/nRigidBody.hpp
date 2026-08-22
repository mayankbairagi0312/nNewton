#pragma once 

#include <iostream>
#include "nTransform.hpp"

#include <memory>   
#include "nCollisionShapes.hpp"
#include "nCollisionTypes.hpp"


namespace nNewton {
	enum class nBodyType : uint8_t { Static, Kinematic, Dynamic };

	struct nMassProperties
	{
		float    Mass = 0.0f;
		nVector3 CenterOfMass = {};
		nMatrix3 Inertia = {};
	};

	struct nRigidBodyInfo {
		nBodyType  TYPE_ = nBodyType::Dynamic;
		nVector3   INIT_VELOCITY_;
		nTransform INIT_TRANSFORM_;
		float      DENSITY_ = 1.0f;     
		bool       OVERRIDE_MASS_ = false;
		float      MASS_ = 1.0f;

	};

	struct nRigidBody
	{
		float    INV_MASS_ = 0.0f;
		nMatrix3 INERTIA_TENSOR_INV_LOCAL_ = {};
		nMatrix3 INERTIA_TENSOR_INV_ = {};
		float MASS_OVERRIDE_ =  0.0f;
		float DENSITY_ = 1.0f;
		nBodyType  TYPE_ = nBodyType::Static;

		nVector3 VELOCITY_;
		nVector3 FORCE_ACC_ = {};
		nVector3   TORQUE_ACC_ = {};
		nTransform TRANSFORM_;

		nVector3 ANGULAR_VELOCITY_ = {};

		nCollisionEntity* ColEnt = nullptr;

		bool IsStatic()    const { return TYPE_ == nBodyType::Static; }
		bool IsKinematic() const { return TYPE_ == nBodyType::Kinematic; }
		bool IsDynamic()   const { return TYPE_ == nBodyType::Dynamic; }

		void ApplyForce(nVector3 force) { if (TYPE_ == nBodyType::Dynamic) FORCE_ACC_ = FORCE_ACC_ + force; }
		void ApplyTorque(nVector3 torque) { if (TYPE_ == nBodyType::Dynamic) TORQUE_ACC_ = TORQUE_ACC_ + torque; }
		void ApplyImpulse(nVector3 impulse, nVector3 worldPoint);

		void SetVelocity(nVector3);


		nVector3 GetForce()const { return FORCE_ACC_; };
		float GetInvMass() const { return INV_MASS_; }
		nVector3 GetVelocity()const { return VELOCITY_; }

		void ClearForces() { FORCE_ACC_ = {}; TORQUE_ACC_ = {}; }

		void Integrate(float dt_);

		nRigidBody() = default;
		explicit nRigidBody(const nRigidBodyInfo& Info_)
			: TYPE_(Info_.TYPE_), VELOCITY_(Info_.INIT_VELOCITY_), TRANSFORM_(Info_.INIT_TRANSFORM_)
			, DENSITY_(Info_.DENSITY_), MASS_OVERRIDE_(Info_.OVERRIDE_MASS_ ? Info_.MASS_ : 0.0f)
		{
		}
	};

}