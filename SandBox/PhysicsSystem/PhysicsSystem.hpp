#include<nNewton/nDynamicsWorld.hpp>
#include<nNewton/nRigidBody.hpp>
#include <nNewton/nTransform.hpp>
#include<nNewton/nMath.hpp>
#include <nNewton/nCollision.hpp>

using namespace nNewton;

class PhysicsSystem
{
public:
	bool INIT_PHYSICS_SYS(nCollisionWorld* collisionWobj)
	{
		m_collisionW = collisionWobj;
		return m_collisionW->INIT_COLLISION_WORLD();

	}

private:
	nCollisionWorld* m_collisionW;
};