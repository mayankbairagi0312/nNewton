#pragma once

#include "DebugRenderer.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include <vector>

constexpr size_t MAX_INSTANCES = 524288;
constexpr size_t MAX_LINES = 524288;

class OpenGLDebugRenderer : public IDebugRenderer
{
public:
	// -- Constructors --
	OpenGLDebugRenderer();
	OpenGLDebugRenderer(const OpenGLDebugRenderer&) = delete;
	OpenGLDebugRenderer& operator=(const OpenGLDebugRenderer&) = delete;
	OpenGLDebugRenderer(OpenGLDebugRenderer&&) noexcept = delete;
	OpenGLDebugRenderer& operator=(OpenGLDebugRenderer&&) noexcept = delete;
	~OpenGLDebugRenderer() override;

	// -- Lifecycle --
	bool InitRenderer(Camera* camera);
	void InitBuffers();

	// -- IDebugRenderer --
	void BeginFrameRenderer() override;
	void EndFrameRenderer() override;
	void DrawLine(const nNewton::nVector3& from, const nNewton::nVector3& to, const nNewton::nVector4& color) override;
	void DrawBox(const nNewton::nVector4& color, const nNewton::nMatrix4& modelMat) override;
	void DrawCircle(const nNewton::nVector4& color, const nNewton::nMatrix4& modelMat) override;
	void ClearRenderer() override;

private:

	struct Vertex {
		nNewton::nVector4 from;
		nNewton::nVector4 color;
	};

	std::vector<Vertex> m_lines;

	GLuint m_VAO = 0;
	GLuint m_VBO = 0;

	GLuint m_PointVBO = 0;
	GLuint m_PointVAO = 0;
	GLuint m_CubeVBO = 0;
	GLuint m_CubeEBO = 0;
	GLuint m_CubeVAO = 0;
	GLuint m_CirVBO = 0;
	GLuint m_CirVAO = 0;
	GLuint m_InstanceVBO = 0;
	GLuint m_InstanceCirVBO = 0;

	std::vector<float> m_instanceCir;
	std::vector<float> m_instanceData;

	size_t m_vertexCount = 0;
	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<Shader> m_InstancedShader;
	Camera* m_Camera = nullptr;
};
