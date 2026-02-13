#pragma once
#include <GLAD/gl.h>
#include <string>
#include <fstream>
#include<sstream>
#include <iostream>
#include <unordered_map>
#include <nNewton/nMath.hpp>

class Shader
{
private:
	GLuint m_programID;
	std::unordered_map<std::string, int> m_uniformLocationCache;
public:

	Shader();
	~Shader();

	bool LoadFromFile(const std::string& vertexPath, const std::string& fragmentPath);
	void Use() const;
	void Clean();

	//uniform setters
	void Set_Bool(const std::string& name, bool value);
	void Set_Int(const std::string& name, int value);
	void Set_Float(const std::string& name, float value);
	void Set_Vec2(const std::string& name, const nNewton::nVector2& value);
	void Set_Vec3(const std::string& name, const nNewton::nVector3& value);
	void Set_Vec4(const std::string& name, const nNewton::nVector4& value);
	//void Set_Mat3(const std::string& name, const glm::mat3& value);
	void Set_Mat4(const std::string& name, const nNewton::nMatrix4& value);;

	GLuint getID() const { return m_programID; }

private:
	std::string ReadFile(const std::string& filepath);
	bool CompileShader(GLuint& shaderID, const std::string& source, GLenum type);
	int GetUniformLocation(const std::string& name);

};