#include <nNewton/nRigidBody.hpp>

namespace nNewton
{
	nRigidBody::nRigidBody(const nRigidBodyInfo& info) noexcept
		: INV_MASS(0.0f)
		, MASS_OVERRIDE(info.OVERRIDE_MASS ? info.MASS : 0.0f)
		, DENSITY(info.DENSITY)
		, TYPE(info.TYPE)
		, VELOCITY(info.INIT_VELOCITY)
		, FORCE_ACC()
		, TORQUE_ACC()
		, TRANSFORM(info.INIT_TRANSFORM)
		, ANGULAR_VELOCITY()
		, ColEnt(nullptr)
	{
	}

	void nRigidBody::Integrate(float dt)
	{
		// Semi-implicit Euler: v += a * dt, then x += v * dt
		const nVector3 linearAccel = GetForce() * GetInvMass();
		SetVelocity(GetVelocity() + linearAccel * dt);

		TRANSFORM.SetPosition(TRANSFORM.GetPosition() + GetVelocity() * dt);
	}

	void nRigidBody::SetVelocity(nVector3 velocity) noexcept
	{
		VELOCITY = velocity;
	}
}
