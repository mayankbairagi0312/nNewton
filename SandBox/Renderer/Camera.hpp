#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
public: 
	enum  class ProjectionType {Perspective , Orthographic};
	
private:
	glm::vec3 m_Position;
	glm::vec3 m_Front;
	glm::vec3 m_Right;
	glm::vec3 m_Up;
	glm::vec3 m_WorldUP;

	float m_Pitch;
	float m_Yaw;
	

	ProjectionType m_ProjectionType;	

	float m_fov;
	float m_aspectRatio;
	float m_farPlane;
	float m_nearPlane;
	
	mutable glm::mat4 m_ViewMatrix;
	mutable glm::mat4 m_ProjectionMatrix;
	mutable bool m_viewDirty;
	mutable bool m_projectionDirty;
	 
public:
	Camera(const glm::vec3& position = glm::vec3(0.0f,0.0f,8.0f),
		const glm::vec3& up = glm::vec3(0.0f,1.0f,0.0f),float yaw = -90.0f,
		float pitch = 0.0f);
	
	glm::mat4 GetViewMatrix()const;
	glm::mat4 GetProjectionMatrix()const;

	void ProcessKeyboard(const glm::vec3& direction, float deltaTime);
	void ProcessMouseMove(float xOffset, float yOffset, bool constrainPitch = true);
	void ProcessMousePan(float xOffset, float yOffset);
	void ProcessMouseScroll(float yOffset, float xOffset);

	void setPosition(const glm::vec3& position);
	void setProjection(ProjectionType type, float fov, float aspectRatio, float nearPlane, float farPlane);
	void setAspectRatio(float aspectRatio);

	const glm::vec3& GetPosition() const { return m_Position; }
	const glm::vec3 GetFront() const { return glm::normalize(m_Front); }
	const glm::vec3& GetUp() const { return m_Up; }
	const glm::vec3 GetRight() const { return glm::normalize(m_Right); }
	float GetFOV() const { return m_fov; }
	float GetFarPlane()const { return m_farPlane; }

private:
	void UpdateCameraVectors();
	void MarkViewDirty();
	void MarkProjectionDirty();
}; 