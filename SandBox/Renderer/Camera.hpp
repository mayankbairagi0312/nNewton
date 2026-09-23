#pragma once
#include <nNewton/nTransform.hpp>

class Camera
{
public:
	enum class ProjectionType { Perspective, Orthographic };

	// -- Constructors --
	explicit Camera(const nNewton::nVector3& position = nNewton::nVector3(0.0f, 5.0f, 15.0f),
		const nNewton::nVector3& up = nNewton::nVector3(0.0f, 1.0f, 0.0f),
		float yaw = -90.0f,
		float pitch = 0.0f);
	Camera(const Camera&) = delete;
	Camera& operator=(const Camera&) = delete;
	Camera(Camera&&) noexcept = delete;
	Camera& operator=(Camera&&) noexcept = delete;
	~Camera() = default;

	// -- Matrices --
	nNewton::nMatrix4 GetViewMatrix() const;
	nNewton::nMatrix4 GetProjectionMatrix() const;

	// -- Input processing --
	void ProcessKeyboard(const nNewton::nVector3& direction, float deltaTime);
	void ProcessMouseMove(float xOffset, float yOffset, bool constrainPitch = true);
	void ProcessMousePan(float xOffset, float yOffset);
	void ProcessMouseScroll(float yOffset, float xOffset);

	// -- Setters --
	void SetPosition(const nNewton::nVector3& position);
	void SetProjection(ProjectionType type, float fov, float aspectRatio, float nearPlane, float farPlane);
	void SetAspectRatio(float aspectRatio);

	// -- Getters --
	const nNewton::nVector3& GetPosition() const noexcept { return m_Position; }
	nNewton::nVector3 GetFront() const { return nNewton::Normalized(m_Front); }
	const nNewton::nVector3& GetUp() const noexcept { return m_Up; }
	nNewton::nVector3 GetRight() const { return nNewton::Normalized(m_Right); }
	float GetFOV() const noexcept { return m_fov; }
	float GetFarPlane() const noexcept { return m_farPlane; }
	float GetNearPlane() const noexcept { return m_nearPlane; }

private:
	void UpdateCameraVectors();
	void MarkViewDirty() noexcept { m_viewDirty = true; }
	void MarkProjectionDirty() noexcept { m_projectionDirty = true; }

	nNewton::nVector3 m_Position;
	nNewton::nVector3 m_Front;
	nNewton::nVector3 m_Right;
	nNewton::nVector3 m_Up;
	nNewton::nVector3 m_WorldUp;

	float m_Pitch;
	float m_Yaw;

	ProjectionType m_ProjectionType;

	float m_fov;
	float m_aspectRatio;
	float m_farPlane;
	float m_nearPlane;

	// Lazily recomputed; mutable because the getters are const.
	mutable nNewton::nMatrix4 m_ViewMatrix;
	mutable nNewton::nMatrix4 m_ProjectionMatrix;
	mutable bool m_viewDirty;
	mutable bool m_projectionDirty;
};
