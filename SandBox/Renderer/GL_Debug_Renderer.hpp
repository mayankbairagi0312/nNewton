#pragma once 

#include "DebugRenderer.hpp"
#include "Shader.hpp"

#include "Camera.hpp"
#include<vector>




class OpneGLDebugRenderer : public IDebugRenderer
{
public:
	OpneGLDebugRenderer();
	~OpneGLDebugRenderer();
	bool init_renderer(Camera* camera);
	void BeginFrameRenderer()override;
	void EndFrameRenderer()override;

	void DrawLine(const nNewton::nVector3& from, const nNewton::nVector3& to, const  nNewton::nVector4& color)override;

	void InitialzedBuf();
	void clearRenderer()override;
private:
	struct vertex {
		nNewton::nVector4 from;

		nNewton::nVector4 color;
	};
	std::vector<vertex> m_lines;

	unsigned int m_VBO;
	unsigned int m_VAO;

	std::unique_ptr<Shader> m_Shader;
	Camera* m_Camera;

};