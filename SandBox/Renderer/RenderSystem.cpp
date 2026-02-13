
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

	std::unique_ptr<DebugRenderer> m_Renderer;
	std::unique_ptr<OpneGLDebugRenderer> m_DebugDrawer;

	
public:

	bool INIT_DEBUG_RENDER(Camera* camera)
	{
		m_Renderer = std::make_unique<DebugRenderer>();
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

		for (auto& [id, ro] : render_Map)
		{
			if (!m_physics.IsValid(id))
				continue;

			const nNewton::nTransform* tr = m_physics.GetTransform(id);

			auto model = nNewton::Translate(tr->GetPosition()) *
				nNewton::to_nMatrix4(tr->GetRotation());

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
			auto center = ((min + max) * 0.5f);
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

};