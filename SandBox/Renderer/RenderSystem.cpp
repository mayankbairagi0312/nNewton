#include "RenderSystem.hpp"
#include <stdexcept>

nRenderSystem::nRenderSystem(Camera* cam, nNewton::nDynamicsWorld* dynamicWorld)
	: m_physics(dynamicWorld)
	, m_collisionWorld(dynamicWorld->GetCollisionWorld())
	, m_Renderer(std::make_shared<DebugRenderer>())
	, m_DebugDrawer(std::make_unique<OpenGLDebugRenderer>())
{
	m_Renderer->SetDrawer(m_DebugDrawer.get());
	if (!InitDebugRender(cam)) throw std::runtime_error("RenderSystem Init Fail !!");
}

//====================== Debug drawing =======================//

void nRenderSystem::DrawBVHTree(nNewton::nAABBTree<nNewton::nCollisionEntity>* tree, int maxDepth)
{
	tree->DebugDrawTree([&](const nNewton::nAABB& aabb, int depth, bool isLeaf, bool isRefit) {

		if (depth > maxDepth) return;

		nNewton::nVector4 color;
		if (isRefit)      color = { 0.85f, 0.1f, 0.2f, 0.0f };
		else if (isLeaf)  color = { 0.1f, 0.1f, 0.8f, 0.0f };
		else              color = { 0.7f, 0.7f, 0.1f, 0.0f };

		float alpha = 1.0f;
		if (maxDepth > 0)
			alpha = 1.0f - (static_cast<float>(depth) / static_cast<float>(maxDepth)) * 0.7f;
		color.w = alpha;

		Debug_DrawAABB(aabb.min, aabb.max, color);

		});
}

bool nRenderSystem::InitDebugRender(Camera* camera)
{
	return m_DebugDrawer->InitRenderer(camera);
}

void nRenderSystem::Start_Debug_Draw()
{
	m_Renderer->BeginFrame();
}

void nRenderSystem::End_Debug_Draw()
{
	m_Renderer->EndFrame();
}

void nRenderSystem::ShutDown_DebugRender()
{
	m_Renderer->Clear();
}

void nRenderSystem::Debug_Render()
{
	const bool isShapes = m_Renderer->IsFlagEnabled(flags::Shapes);
	const bool isAABB = m_Renderer->IsFlagEnabled(flags::AABB);
	const bool isContacts = m_Renderer->IsFlagEnabled(flags::Contacts);

	// BVH wireframes.
	if (isAABB) {
		if (m_Renderer->IsFlagEnabled(flags::BVH_Static))
			DrawBVHTree(m_collisionWorld->GetStaticTree(),
				m_Renderer->GetBVHMaxDepth());

		if (m_Renderer->IsFlagEnabled(flags::BVH_Dynamic))
			DrawBVHTree(m_collisionWorld->GetDynamicTree(),
				m_Renderer->GetBVHMaxDepth());
	}

	for (const render_entity& entity : m_RenderEntities)
	{
		if (!m_physics->IsValid(entity.idx)) continue;

		const nNewton::nTransform* bodyTransform = m_physics->GetTransform(entity.idx);
		if (!bodyTransform)
			continue;

		const nNewton::nCollisionShape* shapeHandle = m_physics->GetColliderShape(entity.idx);
		if (!shapeHandle)
			continue;

		if (isShapes)
		{
			nNewton::nMatrix4 model = nNewton::nTransform::ConstrTRS(
				bodyTransform->GetPosition(),
				bodyTransform->GetRotation(),
				bodyTransform->GetScale()
			);

			if (shapeHandle->type == nNewton::nCollisionShapeType::nBox)
			{
				const auto* boxCollider = m_collisionWorld
					->GetColliderPool()
					.getCollider<nNewton::nBoxShape>(*shapeHandle);

				if (boxCollider)
				{
					const nNewton::nMatrix4 localScale = nNewton::Scale(boxCollider->m_HalfExtents);
					model = model * localScale;
				}

				m_Renderer->DrawBox(entity.color, model);
			}
			else if (shapeHandle->type == nNewton::nCollisionShapeType::nSphere)
			{
				const auto* sphereCollider = m_collisionWorld
					->GetColliderPool()
					.getCollider<nNewton::nSphereShape>(*shapeHandle);

				if (sphereCollider)
				{
					const nNewton::nVector3 radius(sphereCollider->radius, sphereCollider->radius, sphereCollider->radius);
					const nNewton::nMatrix4 localScale = nNewton::Scale(radius);
					model = model * localScale;
				}
				m_Renderer->DrawSphere(model, entity.color);
			}
		}

		if (isContacts)
		{
			// Contact drawing not implemented yet.
		}
	}
}

void nRenderSystem::Debug_DrawAxis(const nNewton::nVector3& camPos)
{
	m_Renderer->DrawAxis(camPos, 128);
	m_Renderer->DrawGrid(32);
}

void nRenderSystem::Debug_DrawAABB(const nNewton::nVector3& min, const nNewton::nVector3& max, const nNewton::nVector4& color)
{
	const nNewton::nVector3 center = (min + max) * 0.5f;
	const nNewton::nVector3 halfSize = (max - min) * 0.5f;

	const nNewton::nMatrix4 model = nNewton::Translate(center) * nNewton::Scale(halfSize);

	m_Renderer->DrawBox(color, model);
}

void nRenderSystem::Debug_DrawContactPoint(const nNewton::nVector3& position, const nNewton::nVector3& normal, const nNewton::nVector4& color)
{
	m_Renderer->DrawPoint(position, color, 0.1f);
	m_Renderer->DrawArrow(position, position + normal, 0.3f, color);
}

//====================== Entity registration =======================//

void nRenderSystem::RegisterEntity(nNewton::nEntity_ID id, nNewton::nVector4 color)
{
	m_RenderEntities.push_back({ id, color });
}

void nRenderSystem::UnregisterEntity(nNewton::nEntity_ID id)
{
	std::erase_if(m_RenderEntities, [id](const render_entity& entity) {
		return entity.idx == id;
	});
}
