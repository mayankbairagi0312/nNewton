#include "DebugRenderer.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include<vector>

class OpneGLDebugRenderer : public IDebugRenderer
{
public:
	OpneGLDebugRenderer() : m_VAO(0), m_VBO(0), m_Shader(std::make_unique<Shader>()), m_Camera(nullptr) {

		
	}
	~OpneGLDebugRenderer() {
		
	}
	bool init_renderer(Camera* camera)
	{	
		m_Camera = camera;

		InitialzedBuf();
		if (!m_Shader->LoadFromFile(
			"assets\\vert.glsl",
			"assets\\frag.glsl"))
		{
			std::cerr << "Debug shader failed iski ma ka barosa to load\n";
			m_Shader.reset();
			return false;
		}


		//glEnable(GL_DEPTH_TEST);
		std::cout << "Renderer initialized successfully" << std::endl;
		return true;
	}
	void BeginFrameRenderer()override {
		m_lines.clear();
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	}
	void EndFrameRenderer()override {
		if (m_lines.empty())
			return;

		if (m_Shader)
			m_Shader->Use();
		else
			std::cout << "Shader failed to load" << std::endl;

		glm::mat4 view = m_Camera->GetViewMatrix();
		glm::mat4 projection = m_Camera->GetProjectionMatrix();

		m_Shader->Set_Mat4("uView", view);
		m_Shader->Set_Mat4("uProjection", projection);

		glDisable(GL_DEPTH_TEST);

		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

		glBufferData(GL_ARRAY_BUFFER, m_lines.size() * sizeof(vertex), m_lines.data(), GL_DYNAMIC_DRAW);

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glDisable(GL_LINE_SMOOTH);
		glDisable(GL_FRAMEBUFFER_SRGB);

		glLineWidth(2.0f);

		glDrawArrays(GL_LINES, 0, m_lines.size());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

	}

	void DrawLine(const glm::vec3& from, const glm::vec3& to, const glm::vec4& color)override {
		
		assert(m_Camera && "GLDebugRenderer: Camera not set");

		

		
		m_lines.push_back({ glm::vec4(from, 1.0f), color });
		m_lines.push_back({ glm::vec4(to,   1.0f), color });

	}
	void InitialzedBuf() {
		
		//create
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);


		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, from));
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, color));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		
		
	}
	void clearRenderer()override
	{
		if (m_VBO) {
			glDeleteBuffers(1, &m_VBO);
			m_VBO = 0;
		}
		if (m_VAO) {
			glDeleteVertexArrays(1, &m_VAO);
			m_VAO = 0;
		}

	}
private:
	struct vertex {
		glm::vec3 from;
	
		glm::vec4 color;
	};
	std::vector<vertex> m_lines;

	unsigned int m_VBO;
	unsigned int m_VAO;

	std::unique_ptr<Shader> m_Shader;
	Camera* m_Camera;

};