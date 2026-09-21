#include "RenderSystem.hpp"
#include <chrono>

nRenderSystem::nRenderSystem(Camera* cam,nNewton::nDynamicsWorld* dynamicW) : m_Renderer(std::make_shared<DebugRenderer>()), m_DebugDrawer(std::make_unique<OpneGLDebugRenderer>()),m_physics(dynamicW)
{
	m_Renderer->SetDrawer(m_DebugDrawer.get());
	if (!INIT_DEBUG_RENDER(cam))throw std::runtime_error("RenderSystem Init Fail !!");
}

void nRenderSystem::DrawBVHTree(nNewton::nAABBTree<nNewton::nCollisionEntity> * tree, int maxDepth )
{
	tree->DebugDrawTree([&](const nNewton::nAABB aabb, int depth, bool isLeaf, bool isRefit) {

		if (depth > maxDepth) return;

		nNewton::nVector4 color;
		if (isRefit) color = { 0.85,0.1,0.2 ,0.0 };
		else if (isLeaf) color = { 0.1,0.1,0.8 ,0.0 };
		else color = { 0.7 ,0.7 , 0.1,0.0 };

		float alpha = 1.0f;

		if (maxDepth > 0)
			alpha = 1.0f - ((float)depth / (float)maxDepth) * 0.7f;
		color.w = alpha;

		Debug_DrawAABB(aabb.min, aabb.max, color);

		/*Debug_DrawAABB(marginAABB, );*/

		});
}

bool nRenderSystem::INIT_DEBUG_RENDER(Camera* camera)
{
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
	double matTime = 0.0;
	double DrawTime = 0.0;
	auto t0 = std::chrono::high_resolution_clock::now();

	for (const auto& n : m_RenderEntities)
	{
		if (!m_physics->IsValid(n.idx))continue;

		const auto* bodyTransform = m_physics->GetTransform(n.idx);
		if (!bodyTransform)
			continue;

		const auto* shapeHandle = m_physics->GetColliderShape(n.idx);
		if (!shapeHandle)
			continue;

		if (IsShapes)
		{
			nNewton::nMatrix4 model = nNewton::nTransform::ConstrTRS(
				bodyTransform->GetPosition(),
				bodyTransform->GetRotation(),
				bodyTransform->GetScale()
			);

			auto* shape = m_physics->GetColliderShape(n.idx);
			if (!shape) return;

		
			if (shapeHandle->type == nNewton::nCollisionShapeType::nBox)
			{
				auto* boxCollider = m_physics->GetCollisionWorld()
					->GetColliderPool()
					.getCollider<nNewton::nBoxShape>(*shapeHandle);

				if (boxCollider)
				{
					nNewton::nVector3 halfExt = boxCollider->m_HalfExtents;
					nNewton::nMatrix4 localScale = nNewton::Scale(halfExt);
					model = model * localScale;   
				}

				m_Renderer->DrawBox(n.color, model);
			}
			else if (shapeHandle->type == nNewton::nCollisionShapeType::nSphere)
			{
				auto* sphereCollider = m_physics->GetCollisionWorld()
					->GetColliderPool()
					.getCollider<nNewton::nSphereShape>(*shapeHandle);

				if (sphereCollider)
				{
					nNewton::nVector3 radi = { sphereCollider->radius,sphereCollider->radius,sphereCollider->radius };
					nNewton::nMatrix4 localScale = nNewton::Scale(radi);
					model = model * localScale;   
				}
				m_Renderer->DrawSphere(model, n.color);
			}
		}

		if (IsContacts)
		{

		}

	}
	auto t1 = std::chrono::high_resolution_clock::now();
}

void nRenderSystem::Debug_DrawAxis(const nNewton::nVector3& camPOS)
{
	m_Renderer->DrawAxis(camPOS, 128);
	m_Renderer->DrawGrid(32);
}
void nRenderSystem::Debug_DrawAABB(const nNewton::nVector3& min_, const nNewton::nVector3& max_, const nNewton::nVector4& color)
{
	nNewton::nVector3 center = (min_ + max_) * 0.5f;
	nNewton::nVector3 halfSize = (max_ - min_) * 0.5f;

	auto model = nNewton::Translate(center) * nNewton::Scale(halfSize);

	m_Renderer->DrawBox(color,model);
}
void nRenderSystem::Debug_DrawContactPoint(const nNewton::nVector3& position, const nNewton::nVector3& normal, const nNewton::nVector4& color)
{
	m_Renderer->DrawPoint(position, color, 0.1f);
	m_Renderer->drawArrow(position, position + normal, 0.3f,
		color);
}

void nRenderSystem::RegisterEntity(nNewton::nEntity_ID id,
	nNewton::nVector4   color)
{
	//render_Map[id] = RenderObject{ type, color };
	m_RenderEntities.push_back({ id,color});
}

void nRenderSystem::UnregisterEntity(nNewton::nEntity_ID id)
{
	std::erase_if(m_RenderEntities, [id](const render_entity& item) {
		return item.idx == id;
		});
}

