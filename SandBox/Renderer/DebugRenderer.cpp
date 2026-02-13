#include "DebugRenderer.hpp"
#include <iostream>
#include <cmath>
DebugRenderer::DebugRenderer() : m_Inframe(false), m_enabled(false) ,m_Drawer(nullptr){};
DebugRenderer::~DebugRenderer(){}



void DebugRenderer::BeginFrame()
{   
    
    m_LineCount = 0;
    
	m_Inframe = true;
	if (m_Drawer == nullptr) {
		std::cerr << "ERROR: m_Drawer is null in BeginFrame()!" << std::endl;
		return;
	}
	m_Drawer->BeginFrameRenderer();
}
void DebugRenderer::DrawSphere(const nNewton::nVector3& Center, const nNewton::nMatrix4& model_mat, const nNewton::nVector4& Color, const float Radius , const uint8_t Segments )
{

    float PI = 3.1459265f;
    float x, y, z;
    uint8_t lati = Segments;
    uint8_t longi = Segments;
    float longiAngle, latiAngle;
    std::vector <nNewton::nVector3> PointsOnSphere;
    for (int i = 0; i <= lati; ++i)
    {
        latiAngle = i*PI/ lati ;
        
        for (int j = 0; j <= longi; ++j)
        {
            longiAngle = 2 * PI - (j *2*PI)/ longi;

            x = Center.x + (Radius * sin(latiAngle)) * cos(longiAngle);
            y = Center.y + Radius * cos(latiAngle); 
            z = Center.z + (Radius * sin(longiAngle)) * sin(latiAngle);
            
            auto vert_ = model_mat * nNewton::nVector4(x, y, z, 1.0f);
            PointsOnSphere.emplace_back(vert_.x,vert_.y,vert_.z);

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

void DebugRenderer::DrawLine(const nNewton::nVector3& from, const nNewton::nVector3& to, const nNewton::nVector4& Color) {
    m_LineCount++;
    m_Drawer->DrawLine(from, to, Color);
    
}
void DebugRenderer::DrawPoint(const nNewton::nVector3 Position, const nNewton::nVector4 Color, const float size )
{
	m_Drawer->DrawPoint(Position, Color, size);
}
void DebugRenderer::DrawBox(const nNewton::nVector3& min, const nNewton::nVector3& max, const nNewton::nVector3& Center, const nNewton::nVector4& Color, const nNewton::nMatrix4& model_mat)
{
    nNewton::nVector3 vert_[8] =
    {
        nNewton::nVector3(min.x, min.y, min.z),
        nNewton::nVector3(max.x, min.y, min.z),
        nNewton::nVector3(max.x, min.y, max.z),
        nNewton::nVector3(min.x, min.y, max.z),

        nNewton::nVector3(min.x, max.y, min.z),
        nNewton::nVector3(max.x, max.y, min.z),
        nNewton::nVector3(max.x, max.y, max.z),
        nNewton::nVector3(min.x, max.y, max.z)
    };

    for(int i = 0; i < 8; ++i)
    {
        nNewton::nVector4 world = model_mat * nNewton::nVector4(vert_[i].x, vert_[i].y, vert_[i].z, 1.0f);
        vert_[i] = nNewton::nVector3(world.x, world.y, world.z);
    }

    DrawLine(vert_[0], vert_[1], Color);
    DrawLine(vert_[1], vert_[2], Color);
    DrawLine(vert_[2], vert_[3], Color);
    DrawLine(vert_[3], vert_[0], Color);

    DrawLine(vert_[4], vert_[5], Color);
    DrawLine(vert_[5], vert_[6], Color);
    DrawLine(vert_[6], vert_[7], Color);
    DrawLine(vert_[7], vert_[4], Color);

    DrawLine(vert_[0], vert_[4], Color);
    DrawLine(vert_[1], vert_[5], Color);
    DrawLine(vert_[2], vert_[6], Color);
    DrawLine(vert_[3], vert_[7], Color);

	DrawPoint(Center, Color);
}

void DebugRenderer::drawArrow(const nNewton::nVector3& from, const nNewton::nVector3& to, float headsize, const nNewton::nVector4& Color)
{
    DrawLine(from, to, Color);

    nNewton::nVector3 dir = nNewton::Normalized(to - from);

    nNewton::nVector3 up(0.0f, 1.0f, 0.0f);

    if (abs(nNewton::DotProduct(up, dir)) > 0.99)
    {
        up = nNewton::nVector3(1, 0, 0);
    }

    nNewton::nVector3 right = nNewton::Normalized(nNewton::CrossProduct(dir, up));
    //up = glm::cross(right, dir);

    nNewton::nVector3 arrowbase = to - dir * headsize;

    nNewton::nVector3 point1 = arrowbase + right * headsize * 0.3f;
    nNewton::nVector3 point2 = arrowbase - right * headsize * 0.3f;

    DrawLine(to , point1, Color);
    DrawLine(to, point2, Color);
}

void DebugRenderer::DrawGrid(const uint16_t GridLength)
{
    for (uint16_t i = 1; i <= GridLength; ++i)
    {   
        if(i%2==0)
        {
            auto xfrom = nNewton::nVector3(i, 0, GridLength);
            auto xto = nNewton::nVector3(i, 0, -GridLength);
            DrawLine(xfrom, xto, nNewton::nVector4(0.7f, 0.7f, 0.7f, .08f));

            auto zfrom = nNewton::nVector3(GridLength, 0, i);
            auto zto = nNewton::nVector3(-GridLength, 0, i);

            DrawLine(zfrom, zto, nNewton::nVector4(0.7f, 0.7f, 0.7f, .08f));
        }

    }
    for (uint16_t i = 1; i <= GridLength; ++i)
    {
        if (i % 2 == 0)
        {
            auto xfrom = nNewton::nVector3(-i, 0, GridLength);
            auto xto = nNewton::nVector3(-i, 0, -GridLength);
            DrawLine(xfrom, xto, nNewton::nVector4(0.7f, 0.7f, 0.7f, .08f));

            auto zfrom = nNewton::nVector3(GridLength, 0, -i);
            auto zto = nNewton::nVector3(-GridLength, 0, -i);

            DrawLine(zfrom, zto, nNewton::nVector4(0.7f, 0.7f, 0.7f, .08f));
        }
    }

}

void DebugRenderer::DrawAxis(const nNewton::nVector3& camPOS, float MaxLength )
{
    // X Axis
    nNewton::nVector3 xfrom(camPOS.x - MaxLength, 0, 0);
    nNewton::nVector3 xto(camPOS.x + MaxLength, 0, 0);
    DrawLine(xfrom, xto, nNewton::nVector4(.7f, .2f, .18f, 0.3f));

    //Y AXIS    
    nNewton::nVector3 yfrom(0,  camPOS.y - MaxLength,0);
    nNewton::nVector3 yto(0, camPOS.y + MaxLength,0);
    DrawLine(yfrom, yto, nNewton::nVector4(.2f, .2f, .6f, 0.3f));

    //Z AXIS
    nNewton::nVector3 zfrom(0, 0, camPOS.z - MaxLength);
    nNewton::nVector3 zto(0, 0, camPOS.z + MaxLength);
    DrawLine(zfrom, zto, nNewton::nVector4(.4f, .7f, .2f, 0.3f));
}

void DebugRenderer::DrawCapsule(const nNewton::nVector3& Center,const float height,const nNewton::nVector4& Color, const float Radius , const uint8_t Segments )
{
   
    float PI = 3.1459265f;
    float x, y, z;
    uint8_t lati = Segments;
    uint8_t longi = Segments;
    float longiAngle, latiAngle;
    std::vector <nNewton::nVector3> PointsOnSphere;

    for (int i = 0; i <= lati/2; ++i)
    {
        latiAngle = i * PI / lati;

        for (int j = 0; j <= longi; ++j)
        {
            longiAngle = 2 * PI - (j * 2 * PI) / longi;

            x = Center.x  + (Radius * sin(latiAngle)) * cos(longiAngle);
            y = Center.y + height/2 + Radius * cos(latiAngle);
            z = Center.z + (Radius * sin(longiAngle)) * sin(latiAngle);

            PointsOnSphere.emplace_back(nNewton::nVector3(x, y, z));

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

            PointsOnSphere.emplace_back(nNewton::nVector3(x, y, z));

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

bool DebugRenderer::IsFlagEnabled(flags flag)const {
    return static_cast<uint32_t>(m_flag & flag) != 0;
}


