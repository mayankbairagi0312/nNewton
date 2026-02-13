#pragma once
#include <iostream>
#include <nNewton/nTransform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
public: 
	enum  class ProjectionType {Perspective , Orthographic};
	
private:
	nNewton::nVector3 m_Position;
	nNewton::nVector3 m_Front;
	nNewton::nVector3 m_Right;
	nNewton::nVector3 m_Up;
	nNewton::nVector3 m_WorldUP;

	float m_Pitch;
	float m_Yaw;
	

	ProjectionType m_ProjectionType;	

	float m_fov;
	float m_aspectRatio;
	float m_farPlane;
	float m_nearPlane;
	
	mutable nNewton::nMatrix4 m_ViewMatrix;
	mutable nNewton::nMatrix4 m_ProjectionMatrix;
	mutable bool m_viewDirty;
	mutable bool m_projectionDirty;
	 
public:
	Camera(const nNewton::nVector3& position = nNewton::nVector3(0.0f,0.0f,8.0f),
		const nNewton::nVector3& up = nNewton::nVector3(0.0f,1.0f,0.0f),float yaw = -90.0f,
		float pitch = 0.0f);
	
	nNewton::nMatrix4 GetViewMatrix()const;
	nNewton::nMatrix4 GetProjectionMatrix()const;

	void ProcessKeyboard(const nNewton::nVector3& direction, float deltaTime);
	void ProcessMouseMove(float xOffset, float yOffset, bool constrainPitch = true);
	void ProcessMousePan(float xOffset, float yOffset);
	void ProcessMouseScroll(float yOffset, float xOffset);

	void setPosition(const nNewton::nVector3& position);
	void setProjection(ProjectionType type, float fov, float aspectRatio, float nearPlane, float farPlane);
	void setAspectRatio(float aspectRatio);

	const nNewton::nVector3& GetPosition() const { return m_Position; }
	const nNewton::nVector3 GetFront() const { return nNewton::Normalized(m_Front); }
	const nNewton::nVector3& GetUp() const { return m_Up; }
	const nNewton::nVector3 GetRight() const { return nNewton::Normalized(m_Right); }
	float GetFOV() const { return m_fov; }
	float GetFarPlane()const { return m_farPlane; }

private:
	void UpdateCameraVectors();
	void MarkViewDirty();
	void MarkProjectionDirty();
}; 