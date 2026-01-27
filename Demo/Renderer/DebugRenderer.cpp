#include "DebugRenderer.hpp"
#include <iostream>

DebugRenderer::DebugRenderer() : m_Inframe(false), m_enabled(false) ,m_Drawer(nullptr){};
DebugRenderer::~DebugRenderer(){}

void DebugRenderer::BeginFrame()
{
	m_Inframe = true;
	if (m_Drawer == nullptr) {
		std::cerr << "ERROR: m_Drawer is null in BeginFrame()!" << std::endl;
		return;
	}
	m_Drawer->BeginFrameRenderer();
}
void DebugRenderer::DrawSphare(const glm::vec3& Center, const glm::vec4& Color, const float Radius ,const uint8_t Segments )
{

	

}

void DebugRenderer::DrawLine(const glm::vec3& from, const glm::vec3& to, const glm::vec4& Color) {
	m_Drawer->DrawLine(from, to, Color);
}
void DebugRenderer::DrawPoint(const glm::vec3 Position, const glm::vec4 Color, const float size )
{
	m_Drawer->DrawPoint(Position, Color, size);
}
void DebugRenderer::DrawBox(const glm::vec3& min, const glm::vec3& max, const glm::vec3& Center, const glm::vec4& Color)
{
    glm::vec3 b0 = glm::vec3(min.x, min.y, min.z);
    glm::vec3 b1 = glm::vec3(max.x, min.y, min.z);
    glm::vec3 b2 = glm::vec3(max.x, min.y, max.z);
    glm::vec3 b3 = glm::vec3(min.x, min.y, max.z);
    
    glm::vec3 t0 = glm::vec3(min.x, max.y, min.z);
    glm::vec3 t1 = glm::vec3(max.x, max.y, min.z);
    glm::vec3 t2 = glm::vec3(max.x, max.y, max.z);
    glm::vec3 t3 = glm::vec3(min.x, max.y, max.z);

    DrawLine(b0, b1, Color);
    DrawLine(b1, b2, Color);
    DrawLine(b2, b3, Color);
    DrawLine(b3, b0, Color);

    DrawLine(t0, t1, Color);
    DrawLine(t1, t2, Color);
    DrawLine(t2, t3, Color);
    DrawLine(t3, t0, Color);

    DrawLine(b0, t0, Color);
    DrawLine(b1, t1, Color);
    DrawLine(b2, t2, Color);
    DrawLine(b3, t3, Color);

	DrawPoint(Center, Color);
}



void DebugRenderer::Endframe()
{
	m_Inframe = false;
	m_Drawer->EndFrameRenderer();
}


void DebugRenderer::SetFlagEnabled()
{

}
void DebugRenderer::SetDisableFlag()
{

}

void DebugRenderer::IsFlagEnabled()const {

}


