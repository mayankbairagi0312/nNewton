#include "DebugRenderer.hpp"
#include <iostream>
#include <cmath>
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
void DebugRenderer::DrawSphere(const glm::vec3& Center, const glm::vec4& Color, const float Radius ,const uint8_t Segments )
{

    float PI = 3.1459265f;
    float x, y, z;
    uint8_t lati = Segments;
    uint8_t longi = Segments;
    float longiAngle, latiAngle;
    std::vector <glm::vec3> PointsOnSphere;
    for (int i = 0; i <= lati; ++i)
    {
        latiAngle = i*PI/ lati ;
        
        for (int j = 0; j <= longi; ++j)
        {
            longiAngle = 2 * PI - (j *2*PI)/ longi;

            x = Center.x + (Radius * sin(latiAngle)) * cos(longiAngle);
            y = Center.y + Radius * cos(latiAngle); 
            z = Center.z + (Radius * sin(longiAngle)) * sin(latiAngle);

            PointsOnSphere.push_back(glm::vec3(x, y, z));

        }

    }

    for (int i = 0; i < lati; ++i)  
    {
        for (int j = 0; j < longi; ++j)  
        {
            int current = i * (longi + 1) + j;
            int nextInLongitude = current + 1;
            int nextInLatitude = (i + 1) * (longi + 1) + j;

            // Draw horizontal lines (same latitude, different longitude)
            DrawLine(PointsOnSphere[current], PointsOnSphere[nextInLongitude], Color);
            DrawLine(PointsOnSphere[current], PointsOnSphere[nextInLatitude], Color);
        }
    }

    
    for (int i = 0; i < lati; ++i)
    {
        int lastInRow = i * (longi + 1) + longi;
        int firstInNextRow = (i + 1) * (longi + 1) + longi;
        DrawLine(PointsOnSphere[lastInRow], PointsOnSphere[firstInNextRow], Color);
    }

    DrawPoint(Center, Color);
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

void DebugRenderer::drawArrow(const glm::vec3& from, const glm::vec3& to,float headsize , const glm::vec4& Color)
{
    DrawLine(from, to, Color);

    glm::vec3 dir = glm::normalize(to - from);

    glm::vec3 up(0.0f, 1.0f, 0.0f);

    if (abs(glm::dot(up, dir)) > 0.99)
    {
        up = glm::vec3(1, 0, 0);
    }

    glm::vec3 right = glm::normalize(glm::cross(dir, up));
    //up = glm::cross(right, dir);

    glm::vec3 arrowbase = to - dir * headsize;

    glm::vec3 point1 = arrowbase + right * headsize * 0.3f;
    glm::vec3 point2 = arrowbase - right * headsize * 0.3f;

    DrawLine(to , point1, Color);
    DrawLine(to, point2, Color);
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


