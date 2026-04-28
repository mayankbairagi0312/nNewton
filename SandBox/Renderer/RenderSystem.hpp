#pragma once

#include <unordered_map>
#include<nNewton/nDynamicsWorld.hpp>
#include<nNewton/nTransform.hpp>
#include<nNewton/nMath.hpp>
#include "GL_Debug_Renderer.hpp"
#include <memory>
#include <nNewton/nAABBTree.hpp>
#include<nNewton/nCollision.hpp>
#include<nNewton/nCollisionShapes.hpp>
#include<nNewton/nBoxShape.hpp>
#include <nNewton/nSphereShape.hpp>
#include "DebugUI/DebugUI.hpp"

class nRenderSystem
{


private:
	enum struct RenderObjectType
	{
		DEBUG_BOX,
		DEBUG_SPHERE,
		DEBUG_CAPSULE,
		DEBUG_PLANE
	};
	struct RenderObject
	{
		RenderObjectType objType;
		nNewton::nVector4 color;
	};


	std::unordered_map<nNewton::nEntity_ID, RenderObject> render_Map;
	nNewton::nDynamicsWorld* m_physics;
	nNewton::nCollisionWorld* m_collisionWorld;
	std::shared_ptr<DebugRenderer> m_Renderer;
	std::unique_ptr<OpneGLDebugRenderer> m_DebugDrawer;
	DebugUIEditor* m_DebugUI;

public:
	void DrawBVHTree(nNewton::nAABBTree* tree, int maxDepth = 10);
	bool INIT_DEBUG_RENDER(Camera* camera, std::shared_ptr<DebugRenderer> Drend, nNewton::nCollisionWorld* collisionW,
		nNewton::nDynamicsWorld* dynamicW, DebugUIEditor* UI);
	void Debug_Render();
	void Debug_DrawAxis(const nNewton::nVector3& camPOS);
	void Debug_DrawAABB(const nNewton::nVector3& min_, const nNewton::nVector3& max_, const nNewton::nVector4& color);
	void Debug_DrawContactPoint(const nNewton::nVector3& position, const nNewton::nVector3& normal, const nNewton::nVector4& color);
	void Debug_DrawShape(const RenderObjectType& objType, const nNewton::nMatrix4& model, const nNewton::nVector4& color);

	void Start_Debug_Draw();
	void End_Debug_Draw();
	void ShutDown_DebugRender();



	void defaultScene();

};




