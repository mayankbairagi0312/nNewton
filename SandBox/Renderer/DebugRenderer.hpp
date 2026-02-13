#pragma once

#include<unordered_map>
#include<nNewton/nDynamicsWorld.hpp>
#include<nNewton/nTransform.hpp>
#include<nNewton/nMath.hpp>
#include <glm/glm.hpp>
#include <memory>

class IDebugRenderer
{
public:
	
	virtual ~IDebugRenderer() = default;

	virtual void DrawLine(const  nNewton::nVector3& from,const nNewton::nVector3& to,const nNewton::nVector4& color) = 0;
	void DrawPoint(const nNewton::nVector3& position, const nNewton::nVector4& color,const  float Size = 0.1f) {
		nNewton::nVector3 offset = nNewton::nVector3(Size,0.0f,0.0f);
		DrawLine(position+offset, position-offset,color);
		offset =  nNewton::nVector3(0.0f,Size, 0.0f);
		DrawLine(position + offset, position - offset, color);
		offset = nNewton::nVector3(0.0f, 0.0f,Size);
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
	
	int GetLineCount() const { return m_LineCount; }

	
	void DrawLine(const nNewton::nVector3& from , const nNewton::nVector3& to ,const nNewton::nVector4& Color );
	void DrawPoint(const nNewton::nVector3 Position, const nNewton::nVector4 Color, const float size = 0.1);
	void DrawBox(const nNewton::nVector3& min, const nNewton::nVector3& max, const nNewton::nVector3& Center, const nNewton::nVector4& Color, const nNewton::nMatrix4& model_mat);
	void DrawSphere(const nNewton::nVector3& Center, const nNewton::nMatrix4& model_mat, const nNewton::nVector4& Color,const float Radius = 1, const uint8_t Segments = 32);
	void DrawCapsule(const nNewton::nVector3& Center,const float, const nNewton::nVector4& Color, const float Radius = 1, const uint8_t Segments = 16);
	void drawArrow(const nNewton::nVector3& from, const nNewton::nVector3& to,float headsize , const nNewton::nVector4& Color);
	void DrawGrid(const uint16_t GridLength);
	void DrawAxis(const nNewton::nVector3& camPOS, float);
	void BeginFrame();
	void Endframe();

	void SetDrawer(IDebugRenderer* Drawer) {
		m_Drawer = Drawer;
	}
	void SetFlag(flags& inflag) {
		m_flag =  inflag;
	}
	void SetFlagEnabled();
	void SetDisableFlag();
	
	bool IsFlagEnabled(flags flag)const;

	const IDebugRenderer* GetDrawer()const { return m_Drawer; }
	const flags GetFlag()const {
		return m_flag;
	}

	bool IsEnabled() const{
		return m_enabled;
	}
	void clear()
	{
		m_Drawer->clearRenderer();
	}

private:
	int m_LineCount;
	IDebugRenderer* m_Drawer;
	flags m_flag;
	bool m_enabled;
	bool m_Inframe;
	

};
