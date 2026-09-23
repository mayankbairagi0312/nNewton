#include "GL_Debug_Renderer.hpp"
#include <cmath>
#include <iostream>

OpenGLDebugRenderer::OpenGLDebugRenderer()
	: m_Shader(std::make_unique<Shader>())
	, m_InstancedShader(std::make_unique<Shader>())
{
}

OpenGLDebugRenderer::~OpenGLDebugRenderer() = default;

bool OpenGLDebugRenderer::InitRenderer(Camera* camera)
{
	m_Camera = camera;

	InitBuffers();
	if (!m_Shader->LoadFromFile(
		"assets\\LineDrawVert.glsl",
		"assets\\LineFrag.glsl"))
	{
		std::cerr << "Debug line shader failed to load\n";
		m_Shader.reset();
		return false;
	}

	if (!m_InstancedShader->LoadFromFile(
		"assets\\vert.glsl",
		"assets\\frag.glsl"))
	{
		std::cerr << "Debug instanced shader failed to load\n";
		m_InstancedShader.reset();
		return false;
	}

	std::cout << "Renderer initialized successfully" << std::endl;
	return true;
}

//====================== IDebugRenderer =======================//

void OpenGLDebugRenderer::BeginFrameRenderer()
{
	m_vertexCount = 0;
	m_lines.clear();
	m_instanceData.clear();
	m_instanceCir.clear();
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLDebugRenderer::EndFrameRenderer()
{
	if (m_vertexCount == 0 && m_instanceData.empty() && m_instanceCir.empty()) return;

	// Batched line pass.
	if (!m_lines.empty() && m_vertexCount > 0)
	{
		if (m_Shader)
			m_Shader->Use();

		const nNewton::nMatrix4 view = m_Camera->GetViewMatrix();
		const nNewton::nMatrix4 projection = m_Camera->GetProjectionMatrix();

		m_Shader->Set_Mat4("uView", view);
		m_Shader->Set_Mat4("uProjection", projection);

		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

		const size_t dataSize = m_vertexCount * sizeof(Vertex);
		glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, m_lines.data());

		glDisable(GL_LINE_SMOOTH);
		glDisable(GL_FRAMEBUFFER_SRGB);

		glLineWidth(2.0f);
		glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(m_vertexCount));
	}

	// Instanced box / circle passes.
	if (!m_instanceData.empty() || !m_instanceCir.empty())
	{
		if (m_InstancedShader)
			m_InstancedShader->Use();

		const nNewton::nMatrix4 view = m_Camera->GetViewMatrix();
		const nNewton::nMatrix4 projection = m_Camera->GetProjectionMatrix();

		m_InstancedShader->Set_Mat4("uView", view);
		m_InstancedShader->Set_Mat4("uProjection", projection);

		if (!m_instanceData.empty())
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);
			const size_t instanceSize = m_instanceData.size() * sizeof(float);
			glBufferSubData(GL_ARRAY_BUFFER, 0, instanceSize, m_instanceData.data());

			// Boxes: 24 indexed line endpoints per instance.
			glBindVertexArray(m_CubeVAO);
			const GLsizei instanceCount = static_cast<GLsizei>(m_instanceData.size() / 20);
			glDrawElementsInstanced(GL_LINES, 24, GL_UNSIGNED_INT, 0, instanceCount);

			// Centers as points.
			glBindVertexArray(m_PointVAO);
			glEnable(GL_PROGRAM_POINT_SIZE);
			glPointSize(4.0f);
			glDrawArraysInstanced(GL_POINTS, 0, 1, instanceCount);

			glBindVertexArray(0);
		}

		if (!m_instanceCir.empty())
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_InstanceCirVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, m_instanceCir.size() * sizeof(float), m_instanceCir.data());

			glBindVertexArray(m_CirVAO);
			const GLsizei circleCount = static_cast<GLsizei>(m_instanceCir.size() / 20);
			glDrawArraysInstanced(GL_LINE_LOOP, 0, 64, circleCount);
			glBindVertexArray(0);
		}
	}

	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);
}

void OpenGLDebugRenderer::DrawLine(const nNewton::nVector3& from, const nNewton::nVector3& to, const nNewton::nVector4& color)
{
	m_lines.push_back({ {from.x, from.y, from.z, 1.0f}, color });
	m_lines.push_back({ {to.x, to.y, to.z, 1.0f}, color });
	m_vertexCount += 2;
}

void OpenGLDebugRenderer::DrawBox(const nNewton::nVector4& color, const nNewton::nMatrix4& modelMat)
{
	// Instance data layout: 16 floats of model matrix + 4 floats of color.
	const float* matPtr = &modelMat.A[0];
	m_instanceData.insert(m_instanceData.end(), matPtr, matPtr + 16);
	m_instanceData.insert(m_instanceData.end(), &color.x, &color.x + 4);
}

void OpenGLDebugRenderer::DrawCircle(const nNewton::nVector4& color, const nNewton::nMatrix4& modelMat)
{
	const float* matPtr = &modelMat.A[0];
	m_instanceCir.insert(m_instanceCir.end(), matPtr, matPtr + 16);
	m_instanceCir.insert(m_instanceCir.end(), &color.x, &color.x + 4);
}

//====================== Buffer setup =======================//

void OpenGLDebugRenderer::InitBuffers()
{
	// Line VAO / VBO.
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	glBufferData(GL_ARRAY_BUFFER, MAX_LINES * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, from));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Circle mesh (64-segment unit circle in XY).
	constexpr int seg = 64;
	std::vector<float> circleVerts;
	circleVerts.reserve(seg * 4);

	for (int i = 0; i < seg; ++i)
	{
		const float theta = 2.0f * nNewton::PI * float(i) / float(seg);
		circleVerts.push_back(cosf(theta));
		circleVerts.push_back(sinf(theta));
		circleVerts.push_back(0.0f);
		circleVerts.push_back(1.0f);
	}

	glGenVertexArrays(1, &m_CirVAO);
	glGenBuffers(1, &m_CirVBO);
	glBindVertexArray(m_CirVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_CirVBO);
	glBufferData(GL_ARRAY_BUFFER, seg * 4 * sizeof(float), circleVerts.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	// Cube
	const float cubeVerts[8][4] = {
		{-1, -1, -1, 1},
		{ 1, -1, -1, 1},
		{ 1, -1,  1, 1},
		{-1, -1,  1, 1},
		{-1,  1, -1, 1},
		{ 1,  1, -1, 1},
		{ 1,  1,  1, 1},
		{-1,  1,  1, 1}
	};

	const unsigned int indices[] = {
		0, 1, 1, 2, 2, 3, 3, 0,
		4, 5, 5, 6, 6, 7, 7, 4,
		0, 4, 1, 5, 2, 6, 3, 7
	};

	glGenVertexArrays(1, &m_CubeVAO);
	glGenBuffers(1, &m_CubeVBO);
	glBindVertexArray(m_CubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_CubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &m_CubeEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_CubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Point mesh.
	const float pointVerts[] = { 0, 0, 0, 1 };

	glGenVertexArrays(1, &m_PointVAO);
	glGenBuffers(1, &m_PointVBO);
	glBindVertexArray(m_PointVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_PointVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pointVerts), pointVerts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	// Instance buffers: 20 floats per instance (16 model + 4 color).
	glGenBuffers(1, &m_InstanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);
	glBufferData(GL_ARRAY_BUFFER, MAX_INSTANCES * 20 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &m_InstanceCirVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_InstanceCirVBO);
	glBufferData(GL_ARRAY_BUFFER, MAX_INSTANCES * 20 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

	// Attrib setup helper: 4 attribute slots of vec4 + a 5th color slot.
	auto setupInstanceAttribs = []()
	{
		for (int i = 0; i < 4; ++i)
		{
			glVertexAttribPointer(1 + i, 4, GL_FLOAT, GL_FALSE, 20 * sizeof(float), (void*)(i * 4 * sizeof(float)));
			glEnableVertexAttribArray(1 + i);
			glVertexAttribDivisor(1 + i, 1);
		}
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 20 * sizeof(float), (void*)(16 * sizeof(float)));
		glEnableVertexAttribArray(5);
		glVertexAttribDivisor(5, 1);
	};

	glBindVertexArray(m_CubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);
	setupInstanceAttribs();

	glBindVertexArray(m_PointVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);
	setupInstanceAttribs();

	glBindVertexArray(m_CirVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_InstanceCirVBO);
	setupInstanceAttribs();

	glBindVertexArray(0);

	m_instanceData.reserve(MAX_INSTANCES * (16 + 4));
	m_instanceCir.reserve(MAX_INSTANCES * (16 + 4));
}

void OpenGLDebugRenderer::ClearRenderer()
{
	if (m_VBO) {
		glDeleteBuffers(1, &m_VBO);
		m_VBO = 0;
	}
	if (m_VAO) {
		glDeleteVertexArrays(1, &m_VAO);
		m_VAO = 0;
	}

	if (m_InstanceVBO) glDeleteBuffers(1, &m_InstanceVBO);
	if (m_InstanceCirVBO) glDeleteBuffers(1, &m_InstanceCirVBO);

	if (m_CubeVBO) glDeleteBuffers(1, &m_CubeVBO);
	if (m_CubeVAO) glDeleteVertexArrays(1, &m_CubeVAO);

	if (m_PointVBO) glDeleteBuffers(1, &m_PointVBO);
	if (m_PointVAO) glDeleteVertexArrays(1, &m_PointVAO);

	if (m_CirVBO) glDeleteBuffers(1, &m_CirVBO);
	if (m_CirVAO) glDeleteVertexArrays(1, &m_CirVAO);

	m_InstanceVBO = m_CubeVBO = m_CubeEBO = m_CubeVAO = m_PointVBO = m_PointVAO = m_CirVBO = m_CirVAO = 0;
}
