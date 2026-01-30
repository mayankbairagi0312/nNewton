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

void DebugRenderer::DrawGrid(const uint16_t GridLength)
{
    for (uint16_t i = 1; i <= GridLength; ++i)
    {   
        if(i%2==0)
        {
            auto xfrom = glm::vec3(i, 0, GridLength);
            auto xto = glm::vec3(i, 0, -GridLength);
            DrawLine(xfrom, xto, glm::vec4(0.7f, 0.7f, 0.7f, .08f));

            auto zfrom = glm::vec3(GridLength, 0, i);
            auto zto = glm::vec3(-GridLength, 0, i);

            DrawLine(zfrom, zto, glm::vec4(0.7f, 0.7f, 0.7f, .08f));
        }

    }
    for (uint16_t i = 1; i <= GridLength; ++i)
    {
        if (i % 2 == 0)
        {
            auto xfrom = glm::vec3(-i, 0, GridLength);
            auto xto = glm::vec3(-i, 0, -GridLength);
            DrawLine(xfrom, xto, glm::vec4(0.7f, 0.7f, 0.7f, .08f));

            auto zfrom = glm::vec3(GridLength, 0, -i);
            auto zto = glm::vec3(-GridLength, 0, -i);

            DrawLine(zfrom, zto, glm::vec4(0.7f, 0.7f, 0.7f, .08f));
        }
    }

}

void DebugRenderer::DrawAxis(const glm::vec3& camPOS , float MaxLength)
{
    // X Axis
    glm::vec3 xfrom(camPOS.x - MaxLength, 0, 0);
    glm::vec3 xto(camPOS.x + MaxLength, 0, 0);
    DrawLine(xfrom, xto, glm::vec4(.7f, .2f, .18f, 0.3f));

    //Y AXIS    
    glm::vec3 yfrom(0,  camPOS.y - MaxLength,0);
    glm::vec3 yto(0, camPOS.y + MaxLength,0);
    DrawLine(yfrom, yto, glm::vec4(.2f, .2f, .6f, 0.3f));

    //Z AXIS
    glm::vec3 zfrom(0, 0, camPOS.z - MaxLength);
    glm::vec3 zto(0, 0, camPOS.z + MaxLength);
    DrawLine(zfrom, zto, glm::vec4(.4f, .7f, .2f, 0.3f));
}

void DebugRenderer::DrawCapsule(const glm::vec3& Center,const float height, const glm::vec4& Color, const float Radius , const uint8_t Segments )
{
   
    float PI = 3.1459265f;
    float x, y, z;
    uint8_t lati = Segments;
    uint8_t longi = Segments;
    float longiAngle, latiAngle;
    std::vector <glm::vec3> PointsOnSphere;

    for (int i = 0; i <= lati/2; ++i)
    {
        latiAngle = i * PI / lati;

        for (int j = 0; j <= longi; ++j)
        {
            longiAngle = 2 * PI - (j * 2 * PI) / longi;

            x = Center.x  + (Radius * sin(latiAngle)) * cos(longiAngle);
            y = Center.y + height/2 + Radius * cos(latiAngle);
            z = Center.z + (Radius * sin(longiAngle)) * sin(latiAngle);

            PointsOnSphere.emplace_back(glm::vec3(x, y, z));

        }

    }
    for (int i = lati / 2 + 1; i <= lati ; ++i)
    {
        latiAngle = i * PI / lati;

        for (int j = 0; j <= longi; ++j)
        {
            longiAngle = 2 * PI - (j * 2 * PI) / longi;

            x = Center.x   + (Radius * sin(latiAngle)) * cos(longiAngle);
            y = Center.y  - height/2 + Radius * cos(latiAngle);
            z = Center.z   + (Radius * sin(longiAngle)) * sin(latiAngle);

            PointsOnSphere.emplace_back(glm::vec3(x, y, z));

        }

    }

    for (int i = 0; i < lati; ++i)
    {
        for (int j = 0; j < longi; ++j)
        {
            int current = i * (longi + 1) + j;
            int nextInLongitude = current + 1;
            int nextInLatitude = (i + 1) * (longi + 1) + j;

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


