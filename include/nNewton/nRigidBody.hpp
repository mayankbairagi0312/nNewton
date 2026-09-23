#pragma once

#include "nMath.hpp"
#include "nTransform.hpp"
#include "nCollisionShapes.hpp"
#include "nCollisionTypes.hpp"

namespace nNewton {

	enum class nBodyType : uint8_t { Static, Kinematic, Dynamic };

	struct nMassProperties
	{
		float Mass = 0.0f;
		nVector3 CenterOfMass = {};
		nMatrix3 Inertia = {};
	};

	struct nRigidBodyInfo {
		nBodyType TYPE = nBodyType::Dynamic;
		nVector3 INIT_VELOCITY = {};
		nTransform INIT_TRANSFORM = {};
		float DENSITY = 1.0f;
		bool OVERRIDE_MASS = false;
		float MASS = 1.0f;
	};

	struct nRigidBody
	{
		// -- Constructors --
		nRigidBody() = default;
		explicit nRigidBody(const nRigidBodyInfo& info) noexcept;
		nRigidBody(const nRigidBody&) = default;
		nRigidBody(nRigidBody&&) noexcept = default;
		nRigidBody& operator=(const nRigidBody&) & = default;
		nRigidBody& operator=(nRigidBody&&) & noexcept = default;
		~nRigidBody() = default;

		// -- Queries --
		bool IsStatic() const noexcept { return TYPE == nBodyType::Static; }
		bool IsKinematic() const noexcept { return TYPE == nBodyType::Kinematic; }
		bool IsDynamic() const noexcept { return TYPE == nBodyType::Dynamic; }

		nVector3 GetForce() const noexcept { return FORCE_ACC; }
		float GetInvMass() const noexcept { return INV_MASS; }
		nVector3 GetVelocity() const noexcept { return VELOCITY; }

		// -- Forces / impulses --
		void ApplyForce(nVector3 force) noexcept { if (TYPE == nBodyType::Dynamic) FORCE_ACC += force; }
		void ApplyTorque(nVector3 torque) noexcept { if (TYPE == nBodyType::Dynamic) TORQUE_ACC += torque; }
		void ApplyImpulse(nVector3 impulse, nVector3 worldPoint);
		void SetVelocity(nVector3 velocity) noexcept;

		void ClearForces() noexcept { FORCE_ACC = {}; TORQUE_ACC = {}; }

		// -- Integration --
		void Integrate(float dt);

		// -- State --
		float INV_MASS = 0.0f;
		nMatrix3 INERTIA_TENSOR_INV_LOCAL = {};
		nMatrix3 INERTIA_TENSOR_INV = {};
		float MASS_OVERRIDE = 0.0f;
		float DENSITY = 1.0f;
		nBodyType TYPE = nBodyType::Static;

		nVector3 VELOCITY = {};
		nVector3 FORCE_ACC = {};
		nVector3 TORQUE_ACC = {};
		nTransform TRANSFORM;

		nVector3 ANGULAR_VELOCITY = {};

		nCollisionEntity* ColEnt = nullptr;
	};
}
