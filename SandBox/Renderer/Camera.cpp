
#include "Camera.hpp"

Camera::Camera(const glm::vec3& position , const glm::vec3& up , float yaw , float pitch ) 
	: m_Position(position) , m_Front(0.0f,0.0f,-1.0f),
	 m_Right(1.0f,0.0f,0.0f) ,m_WorldUP(up),m_Pitch(pitch),m_Yaw(yaw)
	 ,m_ProjectionType(ProjectionType::Perspective),m_fov(45.5f), 
	m_aspectRatio(16.0f/9.0f),m_farPlane(150.0f),m_nearPlane(0.1f)
	, m_viewDirty(true), m_projectionDirty(true),m_ProjectionMatrix(glm::mat4(0)),m_ViewMatrix(glm::mat4(0)) {
	UpdateCameraVectors();

	std::cout << "Camera Position: " << m_Position.x << ", " << m_Position.y << ", " << m_Position.z << "\n";
	std::cout << "Camera Front: " << m_Front.x << ", " << m_Front.y << ", " << m_Front.z << "\n";
	std::cout << "Looking at: " << (m_Position + m_Front).x << ", "
		<< (m_Position + m_Front).y << ", " << (m_Position + m_Front).z << "\n";
}

glm::mat4 Camera:: GetViewMatrix() const
{
	if (m_viewDirty)
	{
		m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
		m_viewDirty = false;
	}
	return m_ViewMatrix;
}

glm::mat4 Camera::GetProjectionMatrix()const
{
	if (m_projectionDirty)
	{
		if (m_ProjectionType == ProjectionType::Perspective)
		{
			m_ProjectionMatrix = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearPlane, m_farPlane);
			
		}
		else
		{
			float halfHeight = 5.0f; // Adjust if needed
			float halfWidth = halfHeight * m_aspectRatio;
			m_ProjectionMatrix = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, m_nearPlane, m_farPlane);
		}
		m_projectionDirty = false;
	}
	return m_ProjectionMatrix;
}

void Camera::ProcessKeyboard(const glm::vec3& direction, float deltaTime)
{
	float cam_velocity = 4.0f * deltaTime;
	printf("Velocity: %.6f, Direction: %.2f %.2f %.2f\n",
		cam_velocity, direction.x, direction.y, direction.z);
	m_Position += direction * cam_velocity;
	printf("New Position: %.2f %.2f %.2f\n", m_Position.x, m_Position.y, m_Position.z);
	MarkViewDirty();
}

void Camera::ProcessMouseMove(float xOffset, float yOffset, bool constrainPitch ) {
	xOffset *= 0.1f;
	yOffset *= 0.1f;

	m_Pitch -= yOffset;
	m_Yaw -= xOffset;

	printf("m_Pitch:%.2f\n", m_Pitch);
	printf("m_Yaw:%.2f\n" , m_Yaw);

	if (constrainPitch) {
		if (m_Pitch > 89.0f) m_Pitch = 89.0f;
		if (m_Pitch < -89.0f) m_Pitch = -89.0f;
	}
	UpdateCameraVectors();
	MarkViewDirty();
}
void Camera::ProcessMousePan(float xOffset, float yOffset)
{
	xOffset *= 0.01f;
	yOffset *= 0.01f;
	glm::vec3 x_Offset (xOffset,xOffset,0.0f);
	glm::vec3 y_Offset(yOffset, yOffset, 0.0f); 
		
	m_Position.x += xOffset;
	m_Position.y -= yOffset;
	std::cout << "Camera Position: " << m_Position.x << ", " << m_Position.y << ", " << m_Position.z << "\n";
	MarkViewDirty();
}

void Camera::ProcessMouseScroll(float yOffset , float xOffset) {
	m_fov -= xOffset - 0.8f;
	m_fov += yOffset - 0.8f;
	
	if (m_fov < 1.0f) m_fov = 1.0f;
	if (m_fov > 90.0f) m_fov = 90.0f;
	printf("===> FOV : %.2f <===\n", m_fov);
	MarkProjectionDirty();
}
void Camera::setPosition(const glm::vec3& position) {
	m_Position = position;
	MarkViewDirty();
}

void Camera::setProjection(ProjectionType type, float fov, float aspectRatio, float nearPlane, float farPlane) {
	m_ProjectionType = type;
	m_fov = fov;
	m_aspectRatio = aspectRatio;
	m_nearPlane = nearPlane;
	m_farPlane = farPlane;
	MarkProjectionDirty();
}
void Camera::setAspectRatio(float aspectRatio)
{
	m_aspectRatio = aspectRatio;
}

void Camera::UpdateCameraVectors()
{
	glm::vec3 front = glm::vec3(0);
	front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	printf("front.x : %f " ,front.x);
	front.y = sin(glm::radians(m_Pitch));
	front.z = cos(glm::radians(m_Pitch)) * sin(glm::radians(m_Yaw));
	m_Front = glm::normalize(front);

	//recalc right and up

	m_Right = glm::normalize(glm::cross(m_Front,m_WorldUP));
	m_Up = glm::normalize(glm::cross(m_Right, m_Front));

}
void Camera::MarkViewDirty()
{
	m_viewDirty = true;
}
void Camera::MarkProjectionDirty() {
	m_projectionDirty = true;
}