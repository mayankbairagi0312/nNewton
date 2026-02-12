
#include<unordered_map>
#include "DebugRenderer.hpp"
#include<nNewton/nDynamicsWorld.hpp>
#include<nNewton/nTransform.hpp>
#include<nNewton/nMath.hpp>
#include <glm/glm.hpp>
#include <memory>



 
class nRenderSystem
{


private:
	struct RenderObject
	{
		flags mesh;
		nNewton::nVector4 color;
	};
	std::unordered_map<nNewton::nEntity_ID, RenderObject> render_Map;
	nNewton::nDynamicsWorld m_physics;
	DebugRenderer* Renderer;
public:

	void DrawMesh(flags DrawFlags, nNewton::nVector4 color, nNewton::nMatrix4 model)
	{
		if (!Renderer->IsFlagEnabled(DrawFlags))
			return;
	
		if (DrawFlags == flags::Shapes)
			{
				nNewton::nVector3 min(-1, -1, -1);
				nNewton::nVector3 max(1, 1, 1);
				auto center = ((min + max) * 0.5f);
				Renderer->DrawBox(min, max, center, color, model);
			}
		}
		
	}

	void render()
	{

		for (auto& [id, ro] : render_Map)
		{
			if (!m_physics.IsValid(id))
				continue;

			const nNewton::nTransform* tr = m_physics.GetTransform(id);

			auto model = nNewton::Translate(tr->GetPosition()) *
				nNewton::to_nMatrix4(tr->GetRotation());

			DrawMesh(ro.mesh, ro.color, model);
		}
	}
};