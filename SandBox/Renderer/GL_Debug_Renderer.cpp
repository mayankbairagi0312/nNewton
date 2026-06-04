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
	m_instanceCir.clear();
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}
void OpneGLDebugRenderer::EndFrameRenderer() {
	if (m_vertexCount == 0 && m_instanceData.empty() && m_instanceCir.empty()) return;
	
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

		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

		size_t dataSize = m_vertexCount * sizeof(vertex);


		glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, m_lines.data());

		glDisable(GL_LINE_SMOOTH);
		glDisable(GL_FRAMEBUFFER_SRGB);

		glLineWidth(2.0f);

		glDrawArrays(GL_LINES, 0, m_vertexCount);
	
	}
	
	if (!m_instanceData.empty() || !m_instanceCir.empty())
	{
		if (m_InstancedShader)
			m_InstancedShader->Use();
		else
			std::cout << "Shader failed to load" << std::endl;

		nNewton::nMatrix4 view = m_Camera->GetViewMatrix();
		nNewton::nMatrix4 projection = m_Camera->GetProjectionMatrix();

		m_InstancedShader->Set_Mat4("uView", view);
		m_InstancedShader->Set_Mat4("uProjection", projection);

		if (!m_instanceData.empty())
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);
			size_t instanceSize = (m_instanceData.size()) * sizeof(float);
		
			glBufferSubData(GL_ARRAY_BUFFER, 0, instanceSize, m_instanceData.data());

			//=================================================
			glBindVertexArray(m_CubeVAO);
			GLsizei instanceCount = (GLsizei)(m_instanceData.size() / 20);
			glDrawElementsInstanced(GL_LINES, 24, GL_UNSIGNED_INT, 0, instanceCount);
			//=================================================
			glBindVertexArray(m_PointVAO);

			glEnable(GL_PROGRAM_POINT_SIZE);
			glPointSize(4.0f);
			glDrawArraysInstanced(GL_POINTS, 0, 1, instanceCount);
			//=================================================

			glBindVertexArray(0);


		}

		if (!m_instanceCir.empty())
		{
			//=================================================
			glBindBuffer(GL_ARRAY_BUFFER, m_InstanceCirVBO);

			glBufferSubData(GL_ARRAY_BUFFER, 0, m_instanceCir.size() * sizeof(float), m_instanceCir.data());


			glBindVertexArray(m_CirVAO);
			GLsizei CirCount = (GLsizei)(m_instanceCir.size() / 20);
			glDrawArraysInstanced(GL_LINE_LOOP, 0, 64, CirCount);
			glBindVertexArray(0);
		}
	}
	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);
	
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

void OpneGLDebugRenderer::DrawCircle(const nNewton::nVector4& Color, const nNewton::nMatrix4& model_mat)
{
	const float* matPtr = (const float*)&model_mat;
	m_instanceCir.insert(m_instanceCir.end(), matPtr, matPtr + 16);
	m_instanceCir.insert(m_instanceCir.end(), &Color.x, &Color.x + 4);
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


	//====================
	
	
	const int seg = 64;
	std::vector<float> circleVerts;
	circleVerts.reserve(seg * 4); 

	for (int i = 0; i < seg; ++i)
	{
		float theta = 2.0f * nNewton::PI * float(i) / float(seg);

		float x = cosf(theta);
		float y = sinf(theta);
		float z = 0.0f;
		float w = 1.0f;

		circleVerts.push_back(x);
		circleVerts.push_back(y);
		circleVerts.push_back(z);
		circleVerts.push_back(w);
	}
	
	glGenVertexArrays(1, &m_CirVAO);
	glGenBuffers(1, &m_CirVBO);
	glBindVertexArray(m_CirVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_CirVBO);
	glBufferData(GL_ARRAY_BUFFER, seg * 4* sizeof(float), circleVerts.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);



	float cubeVerts[8][4] = {
	{-1, -1, -1, 1},  
	{ 1, -1, -1, 1},  
	{ 1, -1,  1, 1},  
	{-1, -1,  1, 1},  
	{-1,  1, -1, 1},  
	{ 1,  1, -1, 1},  
	{ 1,  1,  1, 1},  
	{-1,  1,  1, 1}   
	};

	unsigned int indices[] = {
	0,1, 1,2, 2,3, 3,0,  
	4,5, 5,6, 6,7, 7,4, 
	0,4, 1,5, 2,6, 3,7
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


	float pointVerts[] = { 0,0,0,1 };

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

	glGenBuffers(1, &m_InstanceCirVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_InstanceCirVBO);
	glBufferData(GL_ARRAY_BUFFER, MAX_INSTANCES * (20) * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

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

	glBindVertexArray(m_CirVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_InstanceCirVBO);

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
	m_instanceCir.reserve(MAX_INSTANCES* (16 + 4));
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
	if (m_InstanceCirVBO)glDeleteBuffers(1, &m_InstanceCirVBO);


	if (m_CubeVBO) glDeleteBuffers(1, &m_CubeVBO);
	if (m_CubeVAO) glDeleteVertexArrays(1, &m_CubeVAO);

	if (m_PointVBO) glDeleteBuffers(1, &m_PointVBO);
	if (m_PointVAO) glDeleteVertexArrays(1, &m_PointVAO);

	if (m_CirVBO) glDeleteBuffers(1, &m_CirVBO);
	if (m_CirVAO) glDeleteVertexArrays(1, &m_CirVAO);

	m_InstanceVBO =  m_CubeVBO = m_CubeEBO = m_CubeVAO= m_PointVBO= m_PointVAO = m_CirVBO =m_CirVAO= 0;

}
