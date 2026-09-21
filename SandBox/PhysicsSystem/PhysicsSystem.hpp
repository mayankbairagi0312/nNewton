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
	PhysicsSystem() : m_PhysicsWorld(std::make_unique<nDynamicsWorld>())
	{
		m_PhysicsWorld->GetCollisionWorld()->BuildTrees();
	}

	void UpdatePhysicsSystem(float DETLA_TIME)
	{
		m_PhysicsWorld->Step(DETLA_TIME);
		//m_PhysicsWorld->GetCollisionWorld()->StepCollision();
	}

	nDynamicsWorld* GetPhysicsWorld() {
		return m_PhysicsWorld.get();
	}
	
private:
	std::unique_ptr<nDynamicsWorld> m_PhysicsWorld;

};