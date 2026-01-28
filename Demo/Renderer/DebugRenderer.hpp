#pragma once

#include <glm/glm.hpp>
#include <memory>

class IDebugRenderer
{
public:
	
	virtual ~IDebugRenderer() = default;

	virtual void DrawLine(const glm::vec3& from,const glm::vec3& to,const glm::vec4& color) = 0;
	void DrawPoint(const glm::vec3& position, const glm::vec4& color,const  float Size = 0.1f) {
		glm::vec3 offset = glm::vec3(Size,0.0f,0.0f);
		DrawLine(position+offset, position-offset,color);
		offset = glm::vec3(0.0f,Size, 0.0f);
		DrawLine(position + offset, position - offset, color);
		offset = glm::vec3(0.0f, 0.0f,Size);
		DrawLine(position + offset, position - offset, color);
	}
	

	virtual void BeginFrameRenderer() {};
	virtual void EndFrameRenderer() {};
	virtual void clearRenderer() {};


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
	All				= 0xFFFFFFF

};
inline flags operator&(const flags a ,const flags b)
{
	return static_cast<flags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}
inline flags operator|(const flags a, const flags b)
{
	return static_cast<flags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline bool operator!(flags flag)
{
	return static_cast<uint32_t>(flag)== 0;
}

class  DebugRenderer
{
public:
	DebugRenderer();
	~DebugRenderer();
	

	
	void DrawLine(const glm::vec3& from , const glm::vec3& to ,const glm::vec4& Color );
	void DrawPoint(const glm::vec3 Position, const glm::vec4 Color, const float size = 0.1);
	void DrawBox(const glm::vec3& min, const glm::vec3& max, const glm::vec3& Center, const glm::vec4& Color);
	void DrawSphare(const glm::vec3& Center, const glm::vec4& Color,const float Radius = 1, const uint8_t Segments = 32);
	void drawArrow(const glm::vec3& from, const glm::vec3& to, const glm::vec4& Color);
	void BeginFrame();
	void Endframe();

	void SetDrawer(IDebugRenderer* Drawer) {
		m_Drawer = Drawer;
	}
	void SetFlag(flags& inflag) {
		m_flag = std::make_unique<flags>(inflag);
	}
	void SetFlagEnabled();
	void SetDisableFlag();
	
	void IsFlagEnabled()const;

	const IDebugRenderer* GetDrawer()const { return m_Drawer; }
	const flags* GetFlag()const {
		return m_flag.get();
	}

	bool IsEnabled() const{
		return m_enabled;
	}
	void clear()
	{
		m_Drawer->clearRenderer();
	}

private:
	IDebugRenderer* m_Drawer;
	std::unique_ptr<flags> m_flag;
	bool m_enabled;
	bool m_Inframe;
	

};
