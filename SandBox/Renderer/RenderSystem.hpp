#pragma once

#include <memory>
#include <vector>
#include <nNewton/nDynamicsWorld.hpp>
#include <nNewton/nTransform.hpp>
#include <nNewton/nMath.hpp>
#include <nNewton/nAABBTree.hpp>
#include <nNewton/nCollision.hpp>
#include <nNewton/nCollisionShapes.hpp>
#include <nNewton/nBoxShape.hpp>
#include <nNewton/nSphereShape.hpp>
#include <nNewton/nCollisionTypes.hpp>
#include "GL_Debug_Renderer.hpp"

class Camera;

struct render_entity
{
	nNewton::nEntity_ID idx;
	nNewton::nVector4 color;
};

class nRenderSystem
{
private:
	std::vector<render_entity> m_RenderEntities;
	nNewton::nDynamicsWorld* m_physics;
	nNewton::nCollisionWorld* m_collisionWorld;
	std::shared_ptr<DebugRenderer> m_Renderer;
	std::unique_ptr<OpenGLDebugRenderer> m_DebugDrawer;

public:
	// -- Constructors --
	nRenderSystem(Camera* cam, nNewton::nDynamicsWorld* dynamicWorld);
	nRenderSystem(const nRenderSystem&) = delete;
	nRenderSystem& operator=(const nRenderSystem&) = delete;
	nRenderSystem(nRenderSystem&&) noexcept = delete;
	nRenderSystem& operator=(nRenderSystem&&) noexcept = delete;
	~nRenderSystem() = default;

	// -- Debug drawing --
	void DrawBVHTree(nNewton::nAABBTree<nNewton::nCollisionEntity>* tree, int maxDepth = 10);
	bool InitDebugRender(Camera* camera);
	void Debug_Render();
	void Debug_DrawAxis(const nNewton::nVector3& camPos);
	void Debug_DrawAABB(const nNewton::nVector3& min, const nNewton::nVector3& max, const nNewton::nVector4& color);
	void Debug_DrawContactPoint(const nNewton::nVector3& position, const nNewton::nVector3& normal, const nNewton::nVector4& color);

	// -- Frame lifecycle --
	void Start_Debug_Draw();
	void End_Debug_Draw();
	void ShutDown_DebugRender();

	// -- Entity registration --
	void RegisterEntity(nNewton::nEntity_ID id, nNewton::nVector4 color);
	void UnregisterEntity(nNewton::nEntity_ID id);

	DebugRenderer* GetRenderer() { return m_Renderer.get(); }
};
