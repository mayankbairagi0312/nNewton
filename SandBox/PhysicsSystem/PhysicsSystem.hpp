#pragma once

#include<nNewton/nDynamicsWorld.hpp>
#include<nNewton/nRigidBody.hpp>
#include <nNewton/nTransform.hpp>
#include<nNewton/nMath.hpp>


using namespace nNewton;

enum class SimState { Stopped, Playing, Paused };

class PhysicsSystem
{
public:
	bool INIT_PHYSICS_SYS(nDynamicsWorld* DWorld)
	{
		m_PhysicsWorld = DWorld;
		return m_PhysicsWorld->GetCollisionWorld()->INIT_COLLISION_WORLD();

	}

	void UpdatePhysicsSystem(float DETLA_TIME)
	{
		m_PhysicsWorld->Step(DETLA_TIME);
		m_PhysicsWorld->GetCollisionWorld()->StepCollision();
	}


private:
	nDynamicsWorld * m_PhysicsWorld;
};