#include <nNewton/nRigidBody.hpp>

namespace nNewton
{
	
	void nRigidBody::ApplyForce(nVector3 Force)
	{
		if (IS_STATIC_)return;
		FORCE_ACC_ = FORCE_ACC_ + Force;
	}

	nVector3 nRigidBody::GetForce() const {
		return FORCE_ACC_;
	}

	void nRigidBody::Integrate(float dt_)
	{
		nVector3 linearAccel = GetForce() / GetMass();
		SetVelocity(GetVelocity() + linearAccel*dt_);

		TRANSFORM_.SetPosition(TRANSFORM_.GetPosition() + GetVelocity() * dt_);
	}

	void nRigidBody::SetVelocity(nVector3 velo)
	{
		VELOCITY_ = velo;
	}
	void nRigidBody::ClearForces()
	{
		FORCE_ACC_ = {};
	}

}