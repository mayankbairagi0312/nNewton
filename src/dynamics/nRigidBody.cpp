#include <nNewton/nRigidBody.hpp>

namespace nNewton
{
	
	void nRigidBody::Integrate(float dt_)
	{
		nVector3 linearAccel = GetForce()*GetInvMass();
		SetVelocity(GetVelocity() + linearAccel*dt_);

		TRANSFORM_.SetPosition(TRANSFORM_.GetPosition() + GetVelocity() * dt_);
	}

	void nRigidBody::SetVelocity(nVector3 velo)
	{
		VELOCITY_ = velo;
	}

}