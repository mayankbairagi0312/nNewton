#include "DebugRenderer.hpp"
#include <cmath>
#include <vector>

namespace
{
	const nNewton::nMatrix4 rotX90 = nNewton::RotateX(nNewton::PI * 0.5f);
	const nNewton::nMatrix4 rotZ90 = nNewton::RotateZ(nNewton::PI * 0.5f);
	const nNewton::nMatrix4 rotY90 = nNewton::RotateY(nNewton::PI * 0.5f);
}

DebugRenderer::DebugRenderer() noexcept
	: m_LineCount(0)
	, m_Drawer(nullptr)
	, m_flag(flags::All)
	, m_enabled(false)
	, m_InFrame(false)
	, m_BVHMaxDepth(0)
{
}

DebugRenderer::~DebugRenderer() = default;

//====================== Frame lifecycle =======================//

void DebugRenderer::BeginFrame()
{
	m_LineCount = 0;
	m_InFrame = true;
	if (m_Drawer == nullptr) {
		return;
	}
	m_Drawer->BeginFrameRenderer();
}

void DebugRenderer::EndFrame()
{
	m_InFrame = false;
	m_Drawer->EndFrameRenderer();
}

//====================== Primitive drawing =======================//

void DebugRenderer::DrawLine(const nNewton::nVector3& from, const nNewton::nVector3& to, const nNewton::nVector4& color)
{
	++m_LineCount;
	m_Drawer->DrawLine(from, to, color);
}

void DebugRenderer::DrawPoint(const nNewton::nVector3& position, const nNewton::nVector4& color, float size)
{
	if (!m_InFrame) return;
	m_Drawer->DrawPoint(position, color, size);
}

void DebugRenderer::DrawBox(const nNewton::nVector4& color, const nNewton::nMatrix4& modelMat)
{
	if (!m_InFrame) return;
	m_LineCount += 12;
	m_Drawer->DrawBox(color, modelMat);
}

void DebugRenderer::DrawSphere(const nNewton::nMatrix4& modelMat, const nNewton::nVector4& color)
{
	if (!m_InFrame) return;

	m_Drawer->DrawCircle(color, modelMat);
	m_Drawer->DrawCircle(color, modelMat * rotX90);
	m_Drawer->DrawCircle(color, modelMat * rotY90);
	m_LineCount += 64 * 3;
}

void DebugRenderer::DrawCircle(const nNewton::nVector4& color, const nNewton::nMatrix4& modelMat)
{
	if (!m_InFrame) return;
	m_LineCount += 32;
	m_Drawer->DrawCircle(color, modelMat);
}

void DebugRenderer::DrawPlane(const nNewton::nVector3& center, const nNewton::nVector3& normal, const nNewton::nVector4& color, float size)
{
	nNewton::nVector3 up(0.0f, 1.0f, 0.0f);
	if (fabsf(nNewton::DotProduct(up, normal)) > 0.99f)
	{
		up = nNewton::nVector3(1.0f, 0.0f, 0.0f);
	}

	const nNewton::nVector3 right = nNewton::Normalized(nNewton::CrossProduct(normal, up));
	up = nNewton::Normalized(nNewton::CrossProduct(right, normal));

	const nNewton::nVector3 corner1 = center + (right + up) * size;
	const nNewton::nVector3 corner2 = center + (right - up) * size;
	const nNewton::nVector3 corner3 = center + (-right - up) * size;
	const nNewton::nVector3 corner4 = center + (-right + up) * size;

	DrawLine(corner1, corner2, color);
	DrawLine(corner2, corner3, color);
	DrawLine(corner3, corner4, color);
	DrawLine(corner4, corner1, color);
	DrawLine(corner1, corner3, color);
	DrawLine(corner2, corner4, color);
	DrawPoint(center, color);
}

void DebugRenderer::DrawArrow(const nNewton::nVector3& from, const nNewton::nVector3& to, float headSize, const nNewton::nVector4& color)
{
	DrawLine(from, to, color);

	const nNewton::nVector3 dir = nNewton::Normalized(to - from);

	nNewton::nVector3 up(0.0f, 1.0f, 0.0f);
	if (fabsf(nNewton::DotProduct(up, dir)) > 0.99f)
	{
		up = nNewton::nVector3(1.0f, 0.0f, 0.0f);
	}

	const nNewton::nVector3 right = nNewton::Normalized(nNewton::CrossProduct(dir, up));

	const nNewton::nVector3 arrowBase = to - dir * headSize;
	const nNewton::nVector3 point1 = arrowBase + right * headSize * 0.3f;
	const nNewton::nVector3 point2 = arrowBase - right * headSize * 0.3f;

	DrawLine(to, point1, color);
	DrawLine(to, point2, color);
}

void DebugRenderer::DrawGrid(uint16_t gridLength)
{
	const nNewton::nVector4 gridColor(0.7f, 0.7f, 0.7f, 0.1f);

	for (uint16_t i = 1; i <= gridLength; ++i)
	{
		if (i % 2 == 0)
		{
			const nNewton::nVector3 xFrom(i, 0, gridLength);
			const nNewton::nVector3 xTo(i, 0, -gridLength);
			DrawLine(xFrom, xTo, gridColor);

			const nNewton::nVector3 zFrom(gridLength, 0, i);
			const nNewton::nVector3 zTo(-gridLength, 0, i);
			DrawLine(zFrom, zTo, gridColor);
		}
	}

	for (uint16_t i = 1; i <= gridLength; ++i)
	{
		if (i % 2 == 0)
		{
			const nNewton::nVector3 xFrom(-i, 0, gridLength);
			const nNewton::nVector3 xTo(-i, 0, -gridLength);
			DrawLine(xFrom, xTo, gridColor);

			const nNewton::nVector3 zFrom(gridLength, 0, -i);
			const nNewton::nVector3 zTo(-gridLength, 0, -i);
			DrawLine(zFrom, zTo, gridColor);
		}
	}
}

void DebugRenderer::DrawAxis(const nNewton::nVector3& camPos, float maxLength)
{
	if (!m_InFrame) return;

	// X axis
	const nNewton::nVector3 xFrom(camPos.x - maxLength, 0, 0);
	const nNewton::nVector3 xTo(camPos.x + maxLength, 0, 0);
	DrawLine(xFrom, xTo, nNewton::nVector4(0.7f, 0.2f, 0.18f, 0.5f));

	// Y axis
	const nNewton::nVector3 yFrom(0, camPos.y - maxLength, 0);
	const nNewton::nVector3 yTo(0, camPos.y + maxLength, 0);
	DrawLine(yFrom, yTo, nNewton::nVector4(0.2f, 0.2f, 0.6f, 0.5f));

	// Z axis
	const nNewton::nVector3 zFrom(0, 0, camPos.z - maxLength);
	const nNewton::nVector3 zTo(0, 0, camPos.z + maxLength);
	DrawLine(zFrom, zTo, nNewton::nVector4(0.4f, 0.7f, 0.2f, 0.5f));
}

void DebugRenderer::DrawCapsule(const nNewton::nVector3& center, float height, const nNewton::nVector4& color, float radius, uint8_t segments)
{
	constexpr float PI = 3.1459265f;
	const int lati = segments;
	const int longi = segments;

	// Sphere points for both hemispheres.
	std::vector<nNewton::nVector3> pointsOnSphere;
	pointsOnSphere.reserve((lati + 1) * (longi + 1));

	for (int i = 0; i <= lati / 2; ++i)
	{
		const float latiAngle = i * PI / lati;
		for (int j = 0; j <= longi; ++j)
		{
			const float longiAngle = 2 * PI - (j * 2 * PI) / longi;
			const float x = center.x + (radius * sinf(latiAngle)) * cosf(longiAngle);
			const float y = center.y + height / 2 + radius * cosf(latiAngle);
			const float z = center.z + (radius * sinf(longiAngle)) * sinf(latiAngle);
			pointsOnSphere.emplace_back(x, y, z);
		}
	}

	for (int i = lati / 2 + 1; i <= lati; ++i)
	{
		const float latiAngle = i * PI / lati;
		for (int j = 0; j <= longi; ++j)
		{
			const float longiAngle = 2 * PI - (j * 2 * PI) / longi;
			const float x = center.x + (radius * sinf(latiAngle)) * cosf(longiAngle);
			const float y = center.y - height / 2 + radius * cosf(latiAngle);
			const float z = center.z + (radius * sinf(longiAngle)) * sinf(latiAngle);
			pointsOnSphere.emplace_back(x, y, z);
		}
	}

	for (int i = 0; i < lati; ++i)
	{
		for (int j = 0; j < longi; ++j)
		{
			const int current = i * (longi + 1) + j;
			const int nextInLongitude = current + 1;
			const int nextInLatitude = (i + 1) * (longi + 1) + j;

			DrawLine(pointsOnSphere[current], pointsOnSphere[nextInLongitude], color);
			DrawLine(pointsOnSphere[current], pointsOnSphere[nextInLatitude], color);
		}
	}

	for (int i = 0; i < lati; ++i)
	{
		const int lastInRow = i * (longi + 1) + longi;
		const int firstInNextRow = (i + 1) * (longi + 1) + longi;
		DrawLine(pointsOnSphere[lastInRow], pointsOnSphere[firstInNextRow], color);
	}

	DrawPoint(center, color);
}

//====================== Draw flags =======================//

void DebugRenderer::SetFlag(flags flag)
{
	m_flag = flag;
}

void DebugRenderer::SetFlagEnabled(flags flag)
{
	m_flag = m_flag | flag;
}

void DebugRenderer::SetDisableFlag(flags flag)
{
	m_flag = static_cast<flags>(
		static_cast<uint32_t>(m_flag) & ~static_cast<uint32_t>(flag)
	);
}

bool DebugRenderer::IsFlagEnabled(flags flag) const noexcept
{
	return static_cast<uint32_t>(m_flag & flag) != 0;
}
