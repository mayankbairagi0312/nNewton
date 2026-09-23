#pragma once
#include "nMath.hpp"

namespace nNewton {
	class nTransform
	{
	private:
		nVector3 m_Position;
		nQuaternion m_Rotation;
		nVector3 m_Scale;

	public:
		// -- Constructors --
		nTransform();
		nTransform(nVector3 position, nQuaternion rotation, nVector3 scale);
		nTransform(const nTransform&) = default;
		nTransform(nTransform&&) noexcept = default;
		nTransform& operator=(const nTransform&) = default;
		nTransform& operator=(nTransform&&) noexcept = default;
		~nTransform() = default;

		// -- Setters --
		void SetPosition(const nVector3& position) { m_Position = position; }
		void SetRotation(const nQuaternion& rotation) { m_Rotation = rotation; }
		void SetScale(const nVector3& scale) { m_Scale = scale; }

		// -- Getters --
		nVector3 GetPosition() const noexcept { return m_Position; }
		nQuaternion GetRotation() const noexcept { return m_Rotation; }
		nVector3 GetScale() const noexcept { return m_Scale; }

		// -- Basis vectors --
		nVector3 Right() const { return Vec_Rotate(m_Rotation, nVector3(1.0f, 0.0f, 0.0f)); }
		nVector3 Up() const { return Vec_Rotate(m_Rotation, nVector3(0.0f, 1.0f, 0.0f)); }
		nVector3 Forward() const { return Vec_Rotate(m_Rotation, nVector3(0.0f, 0.0f, 1.0f)); }

		// -- Rotation --
		void Rotate(const nVector3& axis, float radians);
		void Rotate(const nQuaternion& rotation);

		// -- Point / vector transforms --
		nVector3 TransformPt(const nVector3& localPoint) const;
		nVector3 TransformVec(const nVector3& localVector) const;
		nVector3 InvTransformPt(const nVector3& worldPoint) const;
		nVector3 InvTransformVec(const nVector3& worldVector) const;

		// -- Matrix conversion --
		static nMatrix4 ConstrTRS(const nVector3& translation, const nQuaternion& rotation, const nVector3& scale);
		nMatrix4 ToMatrix() const;

		// -- Composition / inversion --
		nTransform operator*(const nTransform& rhs) const;
		static nTransform ComposeTransform(const nTransform& parent, const nTransform& child);
		nTransform Inverse() const;
		void Invert();

		// -- Interpolation --
		static nTransform Lerp(const nTransform& a, const nTransform& b, float t);
	};
}
