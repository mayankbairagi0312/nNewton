#include "GL_Debug_Renderer.hpp"

OpneGLDebugRenderer::OpneGLDebugRenderer() : m_VAO(0), m_VBO(0),m_CubeVAO(0),m_CubeVBO(0),m_InstanceVBO(0),m_PointVAO(0),m_PointVBO(0), m_Shader(std::make_unique<Shader>()),m_InstancedShader(std::make_unique<Shader>()), m_Camera(nullptr) {


}
OpneGLDebugRenderer::~OpneGLDebugRenderer() {

}
bool OpneGLDebugRenderer::init_renderer(Camera* camera)
{
	m_Camera = camera;

	InitialzedBuf();
	if (!m_Shader->LoadFromFile(
		"assets\\LineDrawVert.glsl",
		"assets\\LineFrag.glsl"))
	{
		std::cerr << "Debug shader failed iski ma ka barosa to load\n";
		m_Shader.reset();
		return false;
	}

	if (!m_InstancedShader->LoadFromFile(
		"assets\\vert.glsl",
		"assets\\frag.glsl"))
	{
		std::cerr << "Debug shader failed iski ma ka barosa to load\n";
		m_InstancedShader.reset();
		return false;
	}

	//glEnable(GL_DEPTH_TEST);
	std::cout << "Renderer initialized successfully" << std::endl;
	return true;
}
void OpneGLDebugRenderer::BeginFrameRenderer() {
	//m_lines.clear();
	m_vertexCount = 0;
	m_lines.clear();          
	m_instanceData.clear();
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}
void OpneGLDebugRenderer::EndFrameRenderer() {
	//std::cout << "Lines count: " << m_lines.size() << std::endl;
	if (m_vertexCount == 0 && m_instanceData.empty()) return;	
	
	GLboolean depthTestEnabled;
	glGetBooleanv(GL_DEPTH_TEST, &depthTestEnabled);

	if (!m_lines.empty() && m_vertexCount > 0)
	{

		if (m_Shader)
			m_Shader->Use();
		else
			std::cout << "Shader failed to load" << std::endl;

		nNewton::nMatrix4 view = m_Camera->GetViewMatrix();
		nNewton::nMatrix4 projection = m_Camera->GetProjectionMatrix();

		m_Shader->Set_Mat4("uView", view);
		m_Shader->Set_Mat4("uProjection", projection);

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glLineWidth(2.0f);

		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

		size_t dataSize = m_vertexCount * sizeof(vertex);


		glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, m_lines.data());

		glDisable(GL_LINE_SMOOTH);
		glDisable(GL_FRAMEBUFFER_SRGB);

		glLineWidth(2.0f);

		glDrawArrays(GL_LINES, 0, m_vertexCount);
	
	}
	if (!m_instanceData.empty())
	{
		if (m_InstancedShader)
			m_InstancedShader->Use();
		else
			std::cout << "Shader failed to load" << std::endl;

		nNewton::nMatrix4 view = m_Camera->GetViewMatrix();
		nNewton::nMatrix4 projection = m_Camera->GetProjectionMatrix();

		m_InstancedShader->Set_Mat4("uView", view);
		m_InstancedShader->Set_Mat4("uProjection", projection);

		
		glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);
		size_t instanceSize = (m_instanceData.size()) * sizeof(float);
		glBufferSubData(GL_ARRAY_BUFFER, 0, instanceSize, m_instanceData.data());


		glBindVertexArray(m_CubeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_CubeVBO);

		GLsizei instanceCount = (GLsizei)(m_instanceData.size() / 20);

		glDrawArraysInstanced(GL_LINES, 0, 24, instanceCount);

		glBindVertexArray(m_PointVAO);
		
		glEnable(GL_PROGRAM_POINT_SIZE);  
		glPointSize(4.0f);
		glDrawArraysInstanced(GL_POINTS, 0, 1, instanceCount);

		glBindVertexArray(0);


	}
	glBindVertexArray(0);

	if (depthTestEnabled)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}

inline void OpneGLDebugRenderer::DrawLine(const nNewton::nVector3& from, const nNewton::nVector3& to, const  nNewton::nVector4& color) {

	//if (m_vertexCount > m_lines.size()) {
	//	m_lines.resize(m_lines.size() * 2);
	//}
	//vertex* data = m_lines.data();
	////assert(m_Camera && "GL_DebugRenderer: Camera not set");

	//data[m_vertexCount++] = { {from.x,from.y,from.z, 1.0f}, color };
	//data[m_vertexCount++] = {{to.x, to.y, to.z, 1.0f}, color};

	m_lines.push_back({ {from.x,from.y,from.z,1.0f}, color });
	m_lines.push_back({ {to.x,to.y,to.z,1.0f}, color });
	m_vertexCount += 2;
}


void OpneGLDebugRenderer::Drawbox(const nNewton::nVector4& Color, const nNewton::nMatrix4& model_mat)
{
	const float* matPtr = (const float*)&model_mat; 
	m_instanceData.insert(m_instanceData.end(), matPtr, matPtr + 16);
	m_instanceData.insert(m_instanceData.end(), &Color.x, &Color.x + 4);
}



void OpneGLDebugRenderer::InitialzedBuf() {
	// need to update num when idx have 20 bits  
	
	//create
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);


	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	glBufferData(GL_ARRAY_BUFFER, MAX_LINES * sizeof(vertex), nullptr, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, from));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, color));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	m_lines.resize(MAX_LINES * 2);


	float cubeVerts[] = {
		// bottom face (y = -1)
		-1,-1,-1,1,   1,-1,-1,1,
		 1,-1,-1,1,   1,-1, 1,1,
		 1,-1, 1,1,  -1,-1, 1,1,
		-1,-1, 1,1,  -1,-1,-1,1,

		// top face (y = 1)
		-1, 1,-1,1,   1, 1,-1,1,
		 1, 1,-1,1,   1, 1, 1,1,
		 1, 1, 1,1,  -1, 1, 1,1,
		-1, 1, 1,1,  -1, 1,-1,1,

		// vertical edges (x = -1, x = 1, z = -1, z = 1)
		-1,-1,-1,1,  -1, 1,-1,1,
		 1,-1,-1,1,   1, 1,-1,1,
		 1,-1, 1,1,   1, 1, 1,1,
		-1,-1, 1,1,  -1, 1, 1,1
	};

	glGenVertexArrays(1, &m_CubeVAO);
	glGenBuffers(1, &m_CubeVBO);
	glBindVertexArray(m_CubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_CubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	float pointVerts[] = { 0.5f,0.5f,0.5f,1 };

	glGenVertexArrays(1, &m_PointVAO);
	glGenBuffers(1, &m_PointVBO);
	glBindVertexArray(m_PointVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_PointVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pointVerts), pointVerts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &m_InstanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);
	glBufferData(GL_ARRAY_BUFFER, MAX_INSTANCES*(20)*sizeof(float), nullptr, GL_DYNAMIC_DRAW); // (16 + 4)
	
	glBindVertexArray(m_CubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);
	
	for (int i = 0; i < 4; ++i)
	{
		glVertexAttribPointer(1 + i, 4, GL_FLOAT, GL_FALSE, 20 * sizeof(float), (void*)( i* 4  * sizeof(float) ));
		glEnableVertexAttribArray(i + 1);
		glVertexAttribDivisor(1 + i, 1);
	}
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 20 * sizeof(float), (void*)(16* sizeof(float)));
	glEnableVertexAttribArray(5);
	glVertexAttribDivisor(5, 1);


	glBindVertexArray(m_PointVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);

	for (int i = 0; i < 4; ++i)
	{
		glVertexAttribPointer(1 + i, 4, GL_FLOAT, GL_FALSE, 20 * sizeof(float), (void*)(i * 4 * sizeof(float)));
		glEnableVertexAttribArray(i + 1);
		glVertexAttribDivisor(1 + i, 1);
	}
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 20 * sizeof(float), (void*)(16 * sizeof(float)));
	glEnableVertexAttribArray(5);
	glVertexAttribDivisor(5, 1);

	glBindVertexArray(0);
	m_instanceData.reserve(MAX_INSTANCES * (16 + 4));
}
void OpneGLDebugRenderer::clearRenderer()
{
	if (m_VBO) {
		glDeleteBuffers(1, &m_VBO);
		m_VBO = 0;
	}
	if (m_VAO) {
		glDeleteVertexArrays(1, &m_VAO);
		m_VAO = 0;
	}
	if (m_InstanceVBO)glDeleteBuffers(1, &m_InstanceVBO);
	if (m_CubeVBO) glDeleteBuffers(1, &m_CubeVBO);
	if (m_CubeVAO) glDeleteVertexArrays(1, &m_CubeVAO);
	if (m_PointVBO) glDeleteBuffers(1, &m_PointVBO);
	if (m_PointVAO) glDeleteVertexArrays(1, &m_PointVAO);
	m_InstanceVBO = m_CubeVBO = m_CubeVAO= m_PointVBO= m_PointVAO = 0;

}
