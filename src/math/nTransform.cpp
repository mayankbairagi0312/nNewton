#include <nNewton/nTransform.hpp>

namespace nNewton {

	nTransform::nTransform()
		: m_Position(0.0f, 0.0f, 0.0f)
		, m_Rotation(1.0f, 0.0f, 0.0f, 0.0f)
		, m_Scale(1.0f, 1.0f, 1.0f)
	{
	}

	nTransform::nTransform(nVector3 position, nQuaternion rotation, nVector3 scale)
		: m_Position(position)
		, m_Rotation(rotation)
		, m_Scale(scale)
	{
	}

	void nTransform::Rotate(const nVector3& axis, float radians)
	{
		const nQuaternion rotation = from_AxisAngle(axis, radians);
		m_Rotation = m_Rotation * rotation;
	}

	void nTransform::Rotate(const nQuaternion& rotation)
	{
		m_Rotation = QNormalize(rotation * m_Rotation);
	}

	nVector3 nTransform::TransformPt(const nVector3& localPoint) const
	{
		return m_Position + Vec_Rotate(m_Rotation, localPoint);
	}

	nVector3 nTransform::TransformVec(const nVector3& localVector) const
	{
		return Vec_Rotate(m_Rotation, localVector);
	}

	nVector3 nTransform::InvTransformPt(const nVector3& worldPoint) const
	{
		return Vec_Rotate(QInverse(m_Rotation), worldPoint - m_Position);
	}

	nVector3 nTransform::InvTransformVec(const nVector3& worldVector) const
	{
		return Vec_Rotate(QInverse(m_Rotation), worldVector);
	}

	nMatrix4 nTransform::ConstrTRS(const nVector3& translation, const nQuaternion& rotation, const nVector3& scale)
	{
		nMatrix4 model = to_nMatrix4(rotation);

		// Scale the basis columns (column-major: A[col * 4 + row])
		model.A[0] *= scale.x;
		model.A[1] *= scale.x;
		model.A[2] *= scale.x;

		model.A[4] *= scale.y;
		model.A[5] *= scale.y;
		model.A[6] *= scale.y;

		model.A[8] *= scale.z;
		model.A[9] *= scale.z;
		model.A[10] *= scale.z;

		model.A[12] = translation.x;
		model.A[13] = translation.y;
		model.A[14] = translation.z;

		return model;
	}

	nMatrix4 nTransform::ToMatrix() const
	{
		return ConstrTRS(m_Position, m_Rotation, m_Scale);
	}

	nTransform nTransform::Inverse() const
	{
		nTransform inverse;
		inverse.m_Rotation = QInverse(m_Rotation);
		inverse.m_Position = Vec_Rotate(inverse.m_Rotation, -m_Position);
		inverse.m_Scale = nVector3(1.0f / m_Scale.x, 1.0f / m_Scale.y, 1.0f / m_Scale.z);
		return inverse;
	}

	void nTransform::Invert()
	{
		*this = Inverse();
	}

	nTransform nTransform::operator*(const nTransform& rhs) const
	{
		return ComposeTransform(*this, rhs);
	}

	nTransform nTransform::ComposeTransform(const nTransform& parent, const nTransform& child)
	{
		nTransform composed;
		composed.m_Position = parent.TransformPt(child.m_Position);
		composed.m_Rotation = QNormalize(parent.m_Rotation * child.m_Rotation);
		composed.m_Scale = parent.m_Scale * child.m_Scale;
		return composed;
	}

	nTransform nTransform::Lerp(const nTransform& a, const nTransform& b, float t)
	{
		nTransform result;
		result.m_Position = a.m_Position + (b.m_Position - a.m_Position) * t;
		result.m_Scale = a.m_Scale + (b.m_Scale - a.m_Scale) * t;
		result.m_Rotation = QSlerp(a.m_Rotation, b.m_Rotation, t);
		return result;
	}
}
