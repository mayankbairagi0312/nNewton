#pragma once 

#include "DebugRenderer.hpp"
#include "Shader.hpp"

#include "Camera.hpp"
#include<vector>

constexpr size_t MAX_INSTANCES = 524288;
constexpr size_t MAX_LINES = 524288;

class OpneGLDebugRenderer : public IDebugRenderer
{
public:
	OpneGLDebugRenderer();
	~OpneGLDebugRenderer();
	bool init_renderer(Camera* camera);
	void BeginFrameRenderer()override;
	void EndFrameRenderer()override;

	inline void DrawLine(const nNewton::nVector3& from, const nNewton::nVector3& to, const  nNewton::nVector4& color)override;
	void Drawbox(const nNewton::nVector4& Color, const nNewton::nMatrix4& model_mat)override;
	void DrawCircle(const nNewton::nVector4& Color, const nNewton::nMatrix4& model_mat)override;
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

	GLuint m_PointVBO, m_PointVAO;
	GLuint m_CubeVBO, m_CubeEBO, m_CubeVAO;
	GLuint m_CirVBO, m_CirVAO;
	GLuint m_InstanceVBO;
	GLuint m_InstanceCirVBO;

	std::vector<float> m_instanceCir;
	std::vector<float> m_instanceData;

	size_t m_vertexCount = 0;
	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<Shader> m_InstancedShader;
	Camera* m_Camera;
	
};