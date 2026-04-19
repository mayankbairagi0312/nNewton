
#include <unordered_map>
#include "DebugRenderer.hpp"
#include<nNewton/nDynamicsWorld.hpp>
#include<nNewton/nTransform.hpp>
#include<nNewton/nMath.hpp>
#include "GL_Debug_Renderer.cpp"
#include <memory>
#include <nNewton/nAABBTree.hpp>
#include<nNewton/nCollision.hpp>
#include<nNewton/nCollisionShapes.hpp>
#include<nNewton/nBoxShape.hpp>
#include <nNewton/nSphereShape.hpp>
 
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

	
public:

	void DrawBVHTree(nNewton::nAABBTree* tree, int maxDepth = 10)
	{
		tree->DebugDrawTree([&](const nNewton::nAABB aabb, int depth, bool isLeaf, bool isRefit) {

			if (depth > maxDepth) return;
			
			nNewton::nVector4 color;
			if (isRefit) color = { 0.85,0.1,0.2 ,0.0};
			else if (isLeaf) color = { 0.1,0.1,0.8 ,0.0};
			else color = { 0.7 ,0.7 , 0.1,0.0 };

			float alpha = 1.0f - ((float)depth / (float)maxDepth) * 0.7f;
			color.w = alpha ;

			Debug_DrawAABB(aabb.min, aabb.max, color);

			/*Debug_DrawAABB(marginAABB, );*/
			
			});
	}

	bool INIT_DEBUG_RENDER(Camera* camera , std::shared_ptr<DebugRenderer> Drend,nNewton::nCollisionWorld* collisionW , nNewton::nDynamicsWorld* dynamicW)
	{
		m_Renderer = Drend;
		m_collisionWorld = collisionW;
		m_physics = dynamicW;
		m_DebugDrawer = std::make_unique<OpneGLDebugRenderer>();
		m_Renderer->SetDrawer(m_DebugDrawer.get());

		if (!m_DebugDrawer->init_renderer(camera))
			return false;

		return true;
	}

	void Start_Debug_Draw()
	{
		m_Renderer->BeginFrame();
	}
	void End_Debug_Draw()
	{
		m_Renderer->Endframe();
	}
	void ShutDown_DebugRender()
	{
		m_Renderer->clear();
	}



	void Debug_Render()
	{
		
		m_Renderer->DrawGrid(128);
		auto IsShapes = m_Renderer->IsFlagEnabled(flags::Shapes);
		auto IsAABB = m_Renderer->IsFlagEnabled(flags::AABB);
		auto IsContacts = m_Renderer->IsFlagEnabled(flags::Contacts);
		
		auto* dynTree = m_collisionWorld->GetDynamicTree();
		auto* staTree = m_collisionWorld->GetStaticTree();
		

		/*auto* dynRoot = m_collisionWorld->GetDynamicTree()->GetRoot();
		auto* staRoot = m_collisionWorld->GetStaticTree()->GetRoot();
		*/

		DrawBVHTree(dynTree);
		DrawBVHTree(staTree);
		for (auto& [id, ro] : render_Map)
		{
			if (!m_physics->IsValid(id))
				continue;

			const nNewton::nTransform* tr = m_physics->GetTransform(id);

			auto model = nNewton::Translate(tr->GetPosition()) *
				nNewton::to_nMatrix4(tr->GetRotation())*nNewton::Scale(tr->GetScale());

			
			if (IsShapes)
			{
				Debug_DrawShape(ro.objType, model, ro.color);
			}
			if (IsAABB)
			{
			
			}
			if (IsContacts)
			{

			}
		}
	}

	
	void Debug_DrawAxis(const nNewton::nVector3& camPOS)
	{
		m_Renderer->DrawAxis(camPOS,128);
	}
	void Debug_DrawAABB(const nNewton::nVector3& min_, const nNewton::nVector3& max_, const nNewton::nVector4& color)
	{
		nNewton::nVector3 center = (min_ + max_) * 0.5f;
		nNewton::nVector3 halfSize = (max_ - min_) * 0.5f; 

		auto model = nNewton::Translate(center) * nNewton::Scale(halfSize);

		m_Renderer->DrawBox(
			nNewton::nVector3(-1, -1, -1),  
			nNewton::nVector3(1, 1, 1), 
			nNewton::nVector3(0, 0, 0), 
			color,
			model
		);
	}
	void Debug_DrawContactPoint(const nNewton::nVector3& position, const nNewton::nVector3& normal, const nNewton::nVector4& color)
	{
		m_Renderer->DrawPoint(position, color, 0.1f);
		m_Renderer->drawArrow(position, position + normal, 0.3f,
			color);
	}

	void Debug_DrawShape(const RenderObjectType& objType, const nNewton::nMatrix4& model, const nNewton::nVector4& color)
	{
		switch (objType)
		{
		case RenderObjectType::DEBUG_BOX:
		{
			nNewton::nVector3 min(-1, -1, -1);
			nNewton::nVector3 max(1, 1, 1);
			auto center = nNewton::nVector3(model.A[12], model.A[13], model.A[14]);
			m_Renderer->DrawBox(min, max, center, color, model);
			break;
		}
		case RenderObjectType::DEBUG_SPHERE:
		{
			nNewton::nVector3 s_center(0, 0, 0);
			m_Renderer->DrawSphere(s_center, model, color);
			break;
		}
		case RenderObjectType::DEBUG_CAPSULE:
		{
			break;
		}
		case RenderObjectType::DEBUG_PLANE:
		{
			nNewton::nVector3 p_center(0, 0, 0);
			nNewton::nVector3 normal(0, 1, 0);
			m_Renderer->DrawPlane(p_center, normal, color,5);
			break;
		}
		}
	}

 	// default scene (deafult Cube :) )
	void defaultScene()
	{	
		//render_Map.clear();

		// Create a box 
		nNewton::nRigidBodyInfo DefaultBoxInfo;
		DefaultBoxInfo.MASS_ = 1;
		DefaultBoxInfo.INIT_VELOCITY_ = nNewton::nVector3(0, 0, 0);
		DefaultBoxInfo.INIT_TRANSFORM_ = nNewton::nTransform(nNewton::nVector3(0, 1.0f, 0), nNewton::nQuaternion(), nNewton::nVector3(1, 1, 1));
		DefaultBoxInfo.IS_STATIC_ = false;

		//another box
		nNewton::nRigidBodyInfo DefaultBoxInfo2;
		DefaultBoxInfo2.MASS_ = 1;
		DefaultBoxInfo2.INIT_VELOCITY_ = nNewton::nVector3(0, 0, 0);
		DefaultBoxInfo2.INIT_TRANSFORM_ = nNewton::nTransform(nNewton::nVector3(2, 5.0f, 3), nNewton::nQuaternion(), nNewton::nVector3(2, 1, 1));
		DefaultBoxInfo2.IS_STATIC_ = false;

		//golakar bhuj
		nNewton::nRigidBodyInfo DefaultSphereInfo2;
		DefaultSphereInfo2.MASS_ = 1;
		DefaultSphereInfo2.INIT_VELOCITY_ = nNewton::nVector3(0, 0, 0);
		DefaultSphereInfo2.INIT_TRANSFORM_ = nNewton::nTransform(nNewton::nVector3(-2, 3, 0), nNewton::nQuaternion(), nNewton::nVector3(1,1,1));
		DefaultSphereInfo2.IS_STATIC_ = false;

		//Plane
		nNewton::nRigidBodyInfo DefaultPlaneInfo;
		DefaultPlaneInfo.INIT_TRANSFORM_ = nNewton::nTransform(nNewton::nVector3(0, 0, 0), nNewton::nQuaternion(), nNewton::nVector3(1, 1, 1));
		DefaultPlaneInfo.IS_STATIC_ = true;

		auto boxShape = std::make_shared<nNewton::nBoxShape>(nNewton::nVector3(1,1,1));
		//auto sphereShape = std::make_shared<nNewton::nSphereShape>(1.0f);
		auto planeShape = std::make_shared<nNewton::nBoxShape>(nNewton::nVector3(5, 0.05f, 5));

		nNewton::nEntity_ID boxID = m_physics->Create_Entity(DefaultBoxInfo);
		nNewton::nEntity_ID boxID2 = m_physics->Create_Entity(DefaultBoxInfo2);
		nNewton::nEntity_ID SphereID = m_physics->Create_Entity(DefaultSphereInfo2);
		nNewton::nEntity_ID planeID = m_physics->Create_Entity(DefaultPlaneInfo);

		m_collisionWorld->CreateCollisionEntity(boxID, DefaultBoxInfo.IS_STATIC_, DefaultBoxInfo.INIT_TRANSFORM_, DefaultBoxInfo.INIT_VELOCITY_,boxShape);
		m_collisionWorld->CreateCollisionEntity(boxID2, DefaultBoxInfo2.IS_STATIC_, DefaultBoxInfo2.INIT_TRANSFORM_, DefaultBoxInfo2.INIT_VELOCITY_, boxShape);
		m_collisionWorld->CreateCollisionEntity(SphereID, DefaultSphereInfo2.IS_STATIC_, DefaultSphereInfo2.INIT_TRANSFORM_, DefaultSphereInfo2.INIT_VELOCITY_, boxShape);

		m_collisionWorld->CreateCollisionEntity(planeID, DefaultPlaneInfo.IS_STATIC_, DefaultPlaneInfo.INIT_TRANSFORM_, DefaultPlaneInfo.INIT_VELOCITY_,planeShape);

		render_Map.insert({ boxID, {RenderObjectType::DEBUG_BOX, nNewton::nVector4(0.8f, 0.8f, 0.0f, 1.0f)} });
		render_Map.insert({ boxID2, {RenderObjectType::DEBUG_BOX, nNewton::nVector4(0.6f, 0.8f, 0.3f, 1.0f)} });
		render_Map.insert({ SphereID, {RenderObjectType::DEBUG_SPHERE, nNewton::nVector4(0.9f, 0.2f, 0.3f, 1.0f)} });
		render_Map.insert({ planeID, {RenderObjectType::DEBUG_PLANE, nNewton::nVector4(0.2f, 0.2f, 0.7f, 1.0f)} });
	}
};

