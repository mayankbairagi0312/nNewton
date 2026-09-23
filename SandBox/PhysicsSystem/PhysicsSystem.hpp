#pragma once

#include <nNewton/nDynamicsWorld.hpp>
#include <nNewton/nRigidBody.hpp>
#include <nNewton/nTransform.hpp>
#include <nNewton/nMath.hpp>
#include <memory>

enum class SimState { Stopped, Playing, Paused };

class PhysicsSystem
{
public:
	// -- Constructors --
	PhysicsSystem()
		: m_PhysicsWorld(std::make_unique<nNewton::nDynamicsWorld>())
	{
		m_PhysicsWorld->GetCollisionWorld()->BuildTrees();
	}
	PhysicsSystem(const PhysicsSystem&) = delete;
	PhysicsSystem& operator=(const PhysicsSystem&) = delete;
	PhysicsSystem(PhysicsSystem&&) noexcept = default;
	PhysicsSystem& operator=(PhysicsSystem&&) & noexcept = default;
	~PhysicsSystem() = default;

	// -- Simulation --
	void UpdatePhysicsSystem(float deltaTime)
	{
		m_PhysicsWorld->Step(deltaTime);
	}

	nNewton::nDynamicsWorld* GetPhysicsWorld() noexcept
	{
		return m_PhysicsWorld.get();
	}

private:
	std::unique_ptr<nNewton::nDynamicsWorld> m_PhysicsWorld;
};
