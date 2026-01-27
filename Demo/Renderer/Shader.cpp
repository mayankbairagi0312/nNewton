#include "Shader.hpp"

Shader::Shader() : m_programID(0) {

}

Shader::~Shader()
{
	Clean();
}

bool Shader::LoadFromFile(const std::string& vertexPath, const std::string& fragmentPath)
{
	std::string vertexSource = ReadFile(vertexPath);
	std::string fragmentSource= ReadFile(fragmentPath);

	if (vertexSource.empty() || fragmentPath.empty())
	{
		std::cerr << "failed to read shader files: " << vertexPath << "or" << fragmentPath << std::endl;
		return false;
	}

	m_programID = glCreateProgram();
	unsigned int vertShader, fragShader;
	//vert
	if (!CompileShader(vertShader, vertexSource, GL_VERTEX_SHADER)) {
		std::cerr << "Failed to compile vertex shader" << std::endl;
		return false;
	}

	//fragment 
	if (!CompileShader(fragShader, fragmentSource, GL_FRAGMENT_SHADER)) {
		std::cerr << "Failed to compile fragment shader" << std::endl;
		glDeleteShader(vertShader);
		return false;
	}
	glAttachShader(m_programID, vertShader);
	glAttachShader(m_programID, fragShader);
	glLinkProgram(m_programID);
	
	int success;
	char infoLog[512];
	glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(m_programID, 512, NULL, infoLog);
		std::cerr << "Shader program linking failed: " << infoLog << std::endl;
		glDeleteShader(vertShader);
		glDeleteShader(fragShader);
		return false;
	}

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	std::cout << "Shader loaded successfully: " << vertexPath << ", " << fragmentPath << std::endl;
	return true;
}


bool Shader::CompileShader(GLuint& shaderID, const std::string& source, GLenum type)
{
	shaderID = glCreateShader(type);
	const char* sourceCStr = source.c_str();
	glShaderSource(shaderID, 1, &sourceCStr, NULL);
	glCompileShader(shaderID);

	int success;
	char infoLog[512];
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
		std::cerr << "Shader compilation failed: " << infoLog << std::endl;
		return false;
	}
	return true;
}

void Shader::Use() const {
	glUseProgram(m_programID);
}

void Shader::Clean() {
	if (m_programID) {
		glDeleteProgram(m_programID);
		m_programID = 0;
	}
	m_uniformLocationCache.clear();
}

std::string Shader::ReadFile(const std::string& filepath) {
	std::ifstream file(filepath);
	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << filepath << std::endl;
		return "";
	}

	std::stringstream stream;
	stream << file.rdbuf();
	return stream.str();
}

int Shader::GetUniformLocation(const std::string& name) {
	if (m_uniformLocationCache.find(name) != m_uniformLocationCache.end()) {
		return m_uniformLocationCache[name];
	}

	int location = glGetUniformLocation(m_programID, name.c_str());
	if (location == -1) {
		std::cerr << "Warning: Uniform '" << name << "' doesn't exist!" << std::endl;
	}
	m_uniformLocationCache[name] = location;
	return location;
}


void Shader::Set_Bool(const std::string& name, bool value) {
	glUniform1i(GetUniformLocation(name), (int)value);
}

void Shader::Set_Int(const std::string& name, int value) {
	glUniform1i(GetUniformLocation(name), value);
}

void Shader::Set_Float(const std::string& name, float value) {
	glUniform1f(GetUniformLocation(name), value);
}

void Shader::Set_Vec2(const std::string& name, const glm::vec2& value) {
	glUniform2f(GetUniformLocation(name), value.x, value.y);
}

void Shader::Set_Vec3(const std::string& name, const glm::vec3& value) {
	glUniform3f(GetUniformLocation(name), value.x, value.y, value.z);
}

void Shader::Set_Vec4(const std::string& name, const glm::vec4& value){
	glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w);
}

void Shader::Set_Mat3(const std::string& name, const glm::mat3& value) {
	glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}

void Shader::Set_Mat4(const std::string& name, const glm::mat4& value) {
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}