#include "RenderSystem.hpp"

void nRenderSystem::DrawBVHTree(nNewton::nAABBTree * tree, int maxDepth )
{
	tree->DebugDrawTree([&](const nNewton::nAABB aabb, int depth, bool isLeaf, bool isRefit) {

		if (depth > maxDepth) return;

		nNewton::nVector4 color;
		if (isRefit) color = { 0.85,0.1,0.2 ,0.0 };
		else if (isLeaf) color = { 0.1,0.1,0.8 ,0.0 };
		else color = { 0.7 ,0.7 , 0.1,0.0 };

		float alpha = 1.0f - ((float)depth / (float)maxDepth) * 0.7f;
		color.w = alpha;

		Debug_DrawAABB(aabb.min, aabb.max, color);

		/*Debug_DrawAABB(marginAABB, );*/

		});
}

bool nRenderSystem::INIT_DEBUG_RENDER(Camera* camera, std::shared_ptr<DebugRenderer> Drend, nNewton::nCollisionWorld* collisionW, nNewton::nDynamicsWorld* dynamicW)
{
	m_Renderer = Drend;
	m_collisionWorld = collisionW;
	m_physics = dynamicW;
	m_DebugDrawer = std::make_unique<OpneGLDebugRenderer>();
	m_Renderer->SetDrawer(m_DebugDrawer.get());

	//m_DebugUI = UI;


	if (!m_DebugDrawer->init_renderer(camera))
		return false;

	return true;
}

void nRenderSystem::Start_Debug_Draw()
{
	m_Renderer->BeginFrame();
}
void nRenderSystem::End_Debug_Draw()
{
	m_Renderer->Endframe();
}
void nRenderSystem::ShutDown_DebugRender()
{
	m_Renderer->clear();
}



void nRenderSystem::Debug_Render()
{

	m_Renderer->DrawGrid(128);
	auto IsShapes = m_Renderer->IsFlagEnabled(flags::Shapes);
	auto IsAABB = m_Renderer->IsFlagEnabled(flags::AABB);
	auto IsContacts = m_Renderer->IsFlagEnabled(flags::Contacts);

	if (IsAABB) {
		if (m_Renderer->IsFlagEnabled(flags::BVH_Static))
			DrawBVHTree(m_collisionWorld->GetStaticTree(),
				m_Renderer->GetBVHMaxDepth());

		if (m_Renderer->IsFlagEnabled(flags::BVH_Dynamic))
			DrawBVHTree(m_collisionWorld->GetDynamicTree(),
				m_Renderer->GetBVHMaxDepth());
	}


	/*auto* dynRoot = m_collisionWorld->GetDynamicTree()->GetRoot();
	auto* staRoot = m_collisionWorld->GetStaticTree()->GetRoot();
	*/

	for (auto& [id, ro] : render_Map)
	{
		//std::cout << id << "\n";
		if (!m_physics->IsValid(id))
			continue;

		const nNewton::nTransform* tr = m_physics->GetTransform(id);

		auto model = nNewton::Translate(tr->GetPosition()) *
			nNewton::to_nMatrix4(tr->GetRotation()) * nNewton::Scale(tr->GetScale());


		if (IsShapes)
		{
			Debug_DrawShape(id, model, ro.color);
		}
		
		if (IsContacts)
		{

		}
	}
}


void nRenderSystem::Debug_DrawAxis(const nNewton::nVector3& camPOS)
{
	m_Renderer->DrawAxis(camPOS, 128);
}
void nRenderSystem::Debug_DrawAABB(const nNewton::nVector3& min_, const nNewton::nVector3& max_, const nNewton::nVector4& color)
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
void nRenderSystem::Debug_DrawContactPoint(const nNewton::nVector3& position, const nNewton::nVector3& normal, const nNewton::nVector4& color)
{
	m_Renderer->DrawPoint(position, color, 0.1f);
	m_Renderer->drawArrow(position, position + normal, 0.3f,
		color);
}

void nRenderSystem::Debug_DrawShape(nNewton::nEntity_ID id, const nNewton::nMatrix4& model, const nNewton::nVector4& color)
{
	auto shape = m_physics->GetShape(id);
	if (!shape) return;

	switch (shape->GetType())
	{
	case nNewton::nCollisionShapeType::Box:
		m_Renderer->DrawBox(
			nNewton::nVector3(-1, -1, -1),
			nNewton::nVector3(1, 1, 1),
			nNewton::nVector3(model.A[12], model.A[13], model.A[14]),
			color, model);
		break;
		
	case nNewton::nCollisionShapeType::Sphere:
		m_Renderer->DrawSphere({ 0,0,0 }, model, color);
		break;
	}
}


void nRenderSystem::RegisterEntity(nNewton::nEntity_ID id,
	nNewton::nVector4   color)
{
	//render_Map[id] = RenderObject{ type, color };
	render_Map.insert({ id,RenderObject{color} });
}

void nRenderSystem::UnregisterEntity(nNewton::nEntity_ID id)
{
	render_Map.erase(id);
}

