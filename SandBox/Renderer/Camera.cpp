
#include "Camera.hpp"

Camera::Camera(const nNewton::nVector3& position , const nNewton::nVector3& up , float yaw , float pitch )
	: m_Position(position) , m_Front(0.0f,0.0f,-1.0f),
	 m_Right(1.0f,0.0f,0.0f) ,m_WorldUP(up),m_Pitch(pitch),m_Yaw(yaw)
	 ,m_ProjectionType(ProjectionType::Perspective),m_fov(45.5f), 
	m_aspectRatio(16.0f/9.0f),m_farPlane(150.0f),m_nearPlane(0.1f)
	, m_viewDirty(true), m_projectionDirty(true),m_ProjectionMatrix(nNewton::nMatrix4(0)),m_ViewMatrix(nNewton::nMatrix4(0)) {
	UpdateCameraVectors();

	std::cout << "Camera Position: " << m_Position.x << ", " << m_Position.y << ", " << m_Position.z << "\n";
	std::cout << "Camera Front: " << m_Front.x << ", " << m_Front.y << ", " << m_Front.z << "\n";
	std::cout << "Looking at: " << (m_Position + m_Front).x << ", "
		<< (m_Position + m_Front).y << ", " << (m_Position + m_Front).z << "\n";
}

nNewton::nMatrix4 Camera:: GetViewMatrix() const
{
	if (m_viewDirty)
	{
		m_ViewMatrix = nNewton::Look_At(m_Position, m_Position + m_Front, m_Up);
		m_viewDirty = false;
	}
	return m_ViewMatrix;
}

nNewton::nMatrix4 Camera::GetProjectionMatrix()const
{
	if (m_projectionDirty)
	{
		if (m_ProjectionType == ProjectionType::Perspective)
		{
			m_ProjectionMatrix = nNewton::Perspective(nNewton::Radians(m_fov), m_aspectRatio, m_nearPlane, m_farPlane);
			
		}
		else
		{
			float halfHeight = 5.0f; // Adjust if needed
			float halfWidth = halfHeight * m_aspectRatio;
			m_ProjectionMatrix = nNewton::Ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, m_nearPlane, m_farPlane);
		}
		m_projectionDirty = false;
	}
	return m_ProjectionMatrix;
}

void Camera::ProcessKeyboard(const nNewton::nVector3& direction, float deltaTime)
{
	float cam_velocity = 5.0f * deltaTime;
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
	float panSpeed = 0.0045f;
	xOffset *= panSpeed;
	yOffset *= panSpeed;

	
	auto right = nNewton::Normalized(nNewton::CrossProduct(m_Front, m_Up));
	auto up = m_Up;  

	
	m_Position -= right * xOffset;   
	m_Position += up * yOffset;      

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
void Camera::setPosition(const nNewton::nVector3& position) {
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
	auto front = nNewton::nVector3(0);
	front.x = cos(nNewton::Radians(m_Yaw)) * cos(nNewton::Radians(m_Pitch));
	printf("front.x : %f " ,front.x);
	front.y = sin(nNewton::Radians(m_Pitch));
	front.z = cos(nNewton::Radians(m_Pitch)) * sin(nNewton::Radians(m_Yaw));
	m_Front = nNewton::Normalized(front);

	//recalc right and up

	m_Right = nNewton::Normalized(nNewton::CrossProduct(m_Front,m_WorldUP));
	m_Up = nNewton::Normalized(nNewton::CrossProduct(m_Right, m_Front));

}
void Camera::MarkViewDirty()
{
	m_viewDirty = true;
}
void Camera::MarkProjectionDirty() {
	m_projectionDirty = true;
}