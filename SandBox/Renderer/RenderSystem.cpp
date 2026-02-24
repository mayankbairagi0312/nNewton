
#include<unordered_map>
#include "DebugRenderer.hpp"
#include<nNewton/nDynamicsWorld.hpp>
#include<nNewton/nTransform.hpp>
#include<nNewton/nMath.hpp>
#include <glm/glm.hpp>
#include "GL_Debug_Renderer.cpp"
#include <memory>

 
class nRenderSystem
{


private:
	enum struct RenderObjectType
	{
		DEBUG_BOX,
		DEBUG_SPHERE,
		DEBUG_CAPSULE
	};
	struct RenderObject
	{
		RenderObjectType objType;
		nNewton::nVector4 color;
	};

	
	std::unordered_map<nNewton::nEntity_ID, RenderObject> render_Map;
	nNewton::nDynamicsWorld m_physics;

	std::shared_ptr<DebugRenderer> m_Renderer;
	std::unique_ptr<OpneGLDebugRenderer> m_DebugDrawer;

	
public:

	bool INIT_DEBUG_RENDER(Camera* camera , std::shared_ptr<DebugRenderer> Drend)
	{
		
		m_Renderer = Drend;
		
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
		//std::cout << "render_Map size: " << render_Map.size() << std::endl;
		m_Renderer->DrawGrid(128);
		auto IsShapes = m_Renderer->IsFlagEnabled(flags::Shapes);
		auto IsAABB = m_Renderer->IsFlagEnabled(flags::AABB);
		//std::cout << "IsShapes: " << IsShapes << std::endl;

		for (auto& [id, ro] : render_Map)
		{
			if (!m_physics.IsValid(id))
				continue;

			const nNewton::nTransform* tr = m_physics.GetTransform(id);

			auto model = nNewton::Translate(tr->GetPosition()) *
				nNewton::to_nMatrix4(tr->GetRotation())*nNewton::Scale(tr->GetScale());

			//std::cout<< "position : " << tr->GetPosition().x << " " << tr->GetPosition().y << " " << tr->GetPosition().z << std::endl;
			if (IsShapes)
			{
				Debug_DrawShape(ro.objType, model, ro.color);
			}
			if (IsAABB)
			{
				//Debug_DrawAABB( model, color);
			}
		}
	}

	
	void Debug_DrawAxis(const nNewton::nVector3& camPOS)
	{
		m_Renderer->DrawAxis(camPOS,128);
	}
	void Debug_DrawAABB(const nNewton::nMatrix4& model, const nNewton::nVector4& color)
	{
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
		}
	}

	// default scene (deafult Cube :) )
	void defaultScene()
	{	
		//render_Map.clear();


		nNewton::nRigidBodyInfo DefaultBoxInfo;
		DefaultBoxInfo.MASS_ = 1;
		DefaultBoxInfo.INIT_VELOCITY_ = nNewton::nVector3(0, 0, 0);
		DefaultBoxInfo.INIT_TRANSFORM_ = nNewton::nTransform(nNewton::nVector3(0, 0.5f, 0), nNewton::nQuaternion(), nNewton::nVector3(1, 1, 1));
		DefaultBoxInfo.IS_STATIC_ = true;

		nNewton::nEntity_ID boxID = m_physics.Create_Entity(DefaultBoxInfo);

		render_Map.insert({ boxID, {RenderObjectType::DEBUG_BOX, nNewton::nVector4(0.8f, 0.8f, 0.0f, 1.0f)} });
	}
};

