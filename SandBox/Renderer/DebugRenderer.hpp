#pragma once

#include <cstdint>
#include <memory>
#include <nNewton/nDynamicsWorld.hpp>
#include <nNewton/nTransform.hpp>
#include <nNewton/nMath.hpp>

class IDebugRenderer
{
public:
	virtual ~IDebugRenderer() = default;

	virtual void DrawLine(const nNewton::nVector3& from, const nNewton::nVector3& to, const nNewton::nVector4& color) = 0;
	virtual void DrawBox(const nNewton::nVector4& color, const nNewton::nMatrix4& modelMat) {}
	virtual void DrawCircle(const nNewton::nVector4& color, const nNewton::nMatrix4& modelMat) {}
	virtual void BeginFrameRenderer() {}
	virtual void EndFrameRenderer() {}
	virtual void ClearRenderer() {}

	void DrawPoint(const nNewton::nVector3& position, const nNewton::nVector4& color, float size = 0.1f)
	{
		nNewton::nVector3 offset = nNewton::nVector3(size, 0.0f, 0.0f);
		DrawLine(position + offset, position - offset, color);
		offset = nNewton::nVector3(0.0f, size, 0.0f);
		DrawLine(position + offset, position - offset, color);
		offset = nNewton::nVector3(0.0f, 0.0f, size);
		DrawLine(position + offset, position - offset, color);
	}
};

enum class flags : uint32_t
{
	None			= 0,
	Shapes			= 1 << 0,
	AABB			= 1 << 1,
	Contacts		= 1 << 2,
	Joints			= 1 << 3,
	Normals			= 1 << 4,
	Velocity		= 1 << 5,
	CenterOfMass	= 1 << 6,

	BVH_Static		= 1 << 7,
	BVH_Dynamic		= 1 << 8,
	BVH_FatAABB		= 1 << 9,

	All				= 0xFFFFFFF
};

constexpr flags operator&(flags a, flags b) noexcept
{
	return static_cast<flags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

constexpr flags operator|(flags a, flags b) noexcept
{
	return static_cast<flags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

constexpr bool operator!(flags flag) noexcept
{
	return static_cast<uint32_t>(flag) == 0;
}

class DebugRenderer
{
public:
	// -- Constructors --
	DebugRenderer() noexcept;
	DebugRenderer(const DebugRenderer&) = delete;
	DebugRenderer& operator=(const DebugRenderer&) = delete;
	DebugRenderer(DebugRenderer&&) noexcept = delete;
	DebugRenderer& operator=(DebugRenderer&&) noexcept = delete;
	~DebugRenderer();

	// -- Primitive drawing --
	void DrawLine(const nNewton::nVector3& from, const nNewton::nVector3& to, const nNewton::nVector4& color);
	void DrawPoint(const nNewton::nVector3& position, const nNewton::nVector4& color, float size = 0.1f);
	void DrawBox(const nNewton::nVector4& color, const nNewton::nMatrix4& modelMat);
	void DrawSphere(const nNewton::nMatrix4& modelMat, const nNewton::nVector4& color);
	void DrawCapsule(const nNewton::nVector3& center, float height, const nNewton::nVector4& color, float radius = 1.0f, uint8_t segments = 16);
	void DrawArrow(const nNewton::nVector3& from, const nNewton::nVector3& to, float headSize, const nNewton::nVector4& color);
	void DrawPlane(const nNewton::nVector3& center, const nNewton::nVector3& normal, const nNewton::nVector4& color, float size = 1.0f);
	void DrawCircle(const nNewton::nVector4& color, const nNewton::nMatrix4& modelMat);
	void DrawGrid(uint16_t gridLength);
	void DrawAxis(const nNewton::nVector3& camPos, float maxLength);

	// -- Frame lifecycle --
	void BeginFrame();
	void EndFrame();

	void SetDrawer(IDebugRenderer* drawer) noexcept { m_Drawer = drawer; }
	void Clear() { m_Drawer->ClearRenderer(); }

	// -- Draw flags --
	void SetFlag(flags flag);
	void SetFlagEnabled(flags flag);
	void SetDisableFlag(flags flag);
	bool IsFlagEnabled(flags flag) const noexcept;

	// -- Getters --
	int GetLineCount() const noexcept { return m_LineCount; }
	const IDebugRenderer* GetDrawer() const noexcept { return m_Drawer; }
	flags GetFlag() const noexcept { return m_flag; }
	bool IsEnabled() const noexcept { return m_enabled; }

	void SetBVHMaxDepth(int depth) noexcept { m_BVHMaxDepth = depth; }
	int GetBVHMaxDepth() const noexcept { return m_BVHMaxDepth; }

private:
	int m_LineCount = 0;
	IDebugRenderer* m_Drawer = nullptr;
	flags m_flag = flags::All;
	bool m_enabled = false;
	bool m_InFrame = false;
	int m_BVHMaxDepth = 0;
};
