#include <nNewton/nMath.hpp>
#include <cassert>
#include <cmath>
#include <algorithm>

namespace nNewton
{
//====================== VECTOR 2 =======================//

	float nVector2::Length() const noexcept
	{
		return sqrtf(x * x + y * y);
	}

	nVector2 Normalized(const nVector2& a) noexcept
	{
		const float len = a.Length();
		if (len == 0.0f)
			return nVector2(0.0f);
		return a / len;
	}

//====================== VECTOR 3 =======================//

	float nVector3::Length() const noexcept
	{
		return sqrtf(x * x + y * y + z * z);
	}

	nVector3 Normalized(const nVector3& a) noexcept
	{
		const float len = a.Length();
		if (len == 0.0f)
			return nVector3(0.0f);
		return a / len;
	}

	nVector3 Min(const nVector3& a, const nVector3& b) noexcept
	{
		return nVector3(
			std::min(a.x, b.x),
			std::min(a.y, b.y),
			std::min(a.z, b.z)
		);
	}

	nVector3 Max(const nVector3& a, const nVector3& b) noexcept
	{
		return nVector3(
			std::max(a.x, b.x),
			std::max(a.y, b.y),
			std::max(a.z, b.z)
		);
	}

//====================== VECTOR 4 =======================//

	float nVector4::Length() const noexcept
	{
		return sqrtf(x * x + y * y + z * z + w * w);
	}

	nVector4 Normalized(const nVector4& a) noexcept
	{
		const float len = a.Length();
		if (len == 0.0f)
			return nVector4(0.0f);
		return a / len;
	}

//====================== MATRIX 4 =======================//

	nMatrix4 nMatrix4::operator+(const nMatrix4& otr) const
	{
		nMatrix4 result;
		for (int i = 0; i < 16; ++i)
			result.A[i] = A[i] + otr.A[i];
		return result;
	}

	nMatrix4 nMatrix4::operator-(const nMatrix4& otr) const
	{
		nMatrix4 result;
		for (int i = 0; i < 16; ++i)
			result.A[i] = A[i] - otr.A[i];
		return result;
	}

	// Column-major matrix product: A[col * 4 + row]
	nMatrix4 nMatrix4::operator*(const nMatrix4& otr) const
	{
		nMatrix4 result;
		for (int row = 0; row < 4; ++row)
		{
			for (int col = 0; col < 4; ++col)
			{
				result.A[col * 4 + row] =
					A[0 * 4 + row] * otr.A[col * 4 + 0] +
					A[1 * 4 + row] * otr.A[col * 4 + 1] +
					A[2 * 4 + row] * otr.A[col * 4 + 2] +
					A[3 * 4 + row] * otr.A[col * 4 + 3];
			}
		}
		return result;
	}

	nVector4 nMatrix4::operator*(const nVector4& v) const
	{
		return nVector4(
			A[0] * v.x + A[4] * v.y + A[8] * v.z + A[12] * v.w,
			A[1] * v.x + A[5] * v.y + A[9] * v.z + A[13] * v.w,
			A[2] * v.x + A[6] * v.y + A[10] * v.z + A[14] * v.w,
			A[3] * v.x + A[7] * v.y + A[11] * v.z + A[15] * v.w
		);
	}

	float Determinant(const nMatrix4& m) noexcept
	{
		const float* a = m.A;

		return a[0] * (a[5] * (a[10] * a[15] - a[11] * a[14]) -
			       a[9] * (a[6] * a[15] - a[7] * a[14]) +
			       a[13] * (a[6] * a[11] - a[7] * a[10]))

			 - a[1] * (a[4] * (a[10] * a[15] - a[11] * a[14]) -
			       a[8] * (a[6] * a[15] - a[7] * a[14]) +
			       a[12] * (a[6] * a[11] - a[7] * a[10]))

			 + a[2] * (a[4] * (a[9] * a[15] - a[11] * a[13]) -
			       a[8] * (a[5] * a[15] - a[7] * a[13]) +
			       a[12] * (a[5] * a[11] - a[7] * a[9]))

			 - a[3] * (a[4] * (a[9] * a[14] - a[10] * a[13]) -
			       a[8] * (a[5] * a[14] - a[6] * a[13]) +
			       a[12] * (a[5] * a[10] - a[6] * a[9]));
	}

	nMatrix4 nMatrix4::Inverse() const
	{
		const float det = Determinant(*this);
		if (det == 0.0f)
			return Identity4();

		const float invDet = 1.0f / det;

		// Determinant of a 3x3 submatrix
		constexpr auto det3 = [](float a, float b, float c,
					 float d, float e, float f,
					 float g, float h, float i) -> float
		{
			return a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - e * g);
		};

		nMatrix4 result;

		for (int r = 0; r < 4; ++r)
		{
			for (int c = 0; c < 4; ++c)
			{
				float sub[9];
				int idx = 0;
				for (int i = 0; i < 4; ++i)
				{
					if (i == c) continue;
					for (int j = 0; j < 4; ++j)
					{
						if (j == r) continue;
						sub[idx++] = A[i + j * 4];
					}
				}

				float cofactor = det3(sub[0], sub[1], sub[2],
						      sub[3], sub[4], sub[5],
						      sub[6], sub[7], sub[8]);

				// Apply the sign: (-1)^(c+r)
				if ((c + r) & 1)
					cofactor = -cofactor;

				result.A[r + c * 4] = cofactor * invDet;
			}
		}

		return result;
	}

	nMatrix4 RotateX(float rad)
	{
		return Rotate(rad, nVector3(1.0f, 0.0f, 0.0f));
	}

	nMatrix4 RotateY(float rad)
	{
		return Rotate(rad, nVector3(0.0f, 1.0f, 0.0f));
	}

	nMatrix4 RotateZ(float rad)
	{
		return Rotate(rad, nVector3(0.0f, 0.0f, 1.0f));
	}

	nMatrix4 Translate(const nVector3& t)
	{
		nMatrix4 m = Identity4();
		m.A[12] = t.x;
		m.A[13] = t.y;
		m.A[14] = t.z;
		return m;
	}

	nMatrix4 Scale(const nVector3& s)
	{
		return nMatrix4{
			s.x, 0.0f, 0.0f, 0.0f,
			0.0f, s.y, 0.0f, 0.0f,
			0.0f, 0.0f, s.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	}

	nMatrix4 Transpose(const nMatrix4& m) noexcept
	{
		nMatrix4 result;

		for (int c = 0; c < 4; ++c)
		{
			for (int r = 0; r < 4; ++r)
			{
				result.A[c * 4 + r] = m.A[r * 4 + c];
			}
		}

		return result;
	}

	nMatrix4 Rotate(float rad, const nVector3& axis)
	{
		const float c = cosf(rad);
		const float s = sinf(rad);

		const float len = axis.Length();
		const float x = axis.x / len;
		const float y = axis.y / len;
		const float z = axis.z / len;

		return nMatrix4{
			c + x * x * (1.0f - c),
			y * x * (1.0f - c) + z * s,
			z * x * (1.0f - c) - y * s,
			0.0f,

			x * y * (1.0f - c) - z * s,
			c + y * y * (1.0f - c),
			z * y * (1.0f - c) + x * s,
			0.0f,

			x * z * (1.0f - c) + y * s,
			y * z * (1.0f - c) - x * s,
			c + z * z * (1.0f - c),
			0.0f,

			0.0f, 0.0f, 0.0f, 1.0f
		};
	}

	nMatrix4 Look_At(const nVector3& eye, const nVector3& center, const nVector3& up)
	{
		const nVector3 forward = Normalized(center - eye);
		const nVector3 side = Normalized(CrossProduct(forward, up));
		const nVector3 upVec = Normalized(CrossProduct(side, forward));

		return nMatrix4{
			side.x,    upVec.x,    -forward.x,    0.0f,
			side.y,    upVec.y,    -forward.y,    0.0f,
			side.z,    upVec.z,    -forward.z,    0.0f,
			-DotProduct(side, eye), -DotProduct(upVec, eye), DotProduct(forward, eye), 1.0f
		};
	}

	nMatrix4 Perspective(float FOV, float aspect, float nearZ, float farZ)
	{
		const float f = 1.0f / tanf(FOV / 2.0f);

		return nMatrix4{
			f / aspect, 0.0f, 0.0f, 0.0f,
			0.0f, f, 0.0f, 0.0f,
			0.0f, 0.0f, -(farZ + nearZ) / (farZ - nearZ), -1.0f,
			0.0f, 0.0f, -(2.0f * farZ * nearZ) / (farZ - nearZ), 0.0f
		};
	}

	nMatrix4 Ortho(float left, float right, float bottom, float top, float nearZ, float farZ)
	{
		return nMatrix4{
			2.0f / (right - left), 0.0f, 0.0f, 0.0f,

			0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,

			0.0f, 0.0f, -2.0f / (farZ - nearZ), 0.0f,

			-(right + left) / (right - left),
			-(top + bottom) / (top - bottom),
			-(farZ + nearZ) / (farZ - nearZ),
			1.0f
		};
	}

//====================== QUATERNION =======================//

	nQuaternion operator*(float scalar, const nQuaternion& quat) noexcept
	{
		return nQuaternion(scalar * quat.w, scalar * quat.x, scalar * quat.y, scalar * quat.z);
	}

	float nQuaternion::Length() const noexcept
	{
		return sqrtf(w * w + x * x + y * y + z * z);
	}

	nQuaternion QNormalize(const nQuaternion& quat) noexcept
	{
		const float len = quat.Length();
		if (len == 0.0f)
			return nQuaternion(1.0f, 0.0f, 0.0f, 0.0f);
		return nQuaternion(quat.w / len, quat.x / len, quat.y / len, quat.z / len);
	}

	nQuaternion Conjugate(const nQuaternion& quat) noexcept
	{
		return nQuaternion(quat.w, -quat.x, -quat.y, -quat.z);
	}

	nQuaternion QInverse(const nQuaternion& quat) noexcept
	{
		const float lenSq = quat.w * quat.w + quat.x * quat.x + quat.y * quat.y + quat.z * quat.z;
		if (lenSq == 0.0f)
			return nQuaternion(1.0f, 0.0f, 0.0f, 0.0f);

		const nQuaternion conj = Conjugate(quat);

		// Unit quaternion: inverse is the conjugate
		if (fabsf(lenSq - 1.0f) < 1e-6f)
			return conj;

		return nQuaternion(conj.w / lenSq, conj.x / lenSq, conj.y / lenSq, conj.z / lenSq);
	}

	float QDotProduct(const nQuaternion& a, const nQuaternion& b) noexcept
	{
		return a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;
	}

	nQuaternion from_AxisAngle(const nVector3& axisIn, float angleRad)
	{
		const float lenSq = axisIn.x * axisIn.x + axisIn.y * axisIn.y + axisIn.z * axisIn.z;
		if (lenSq == 0.0f)
			return nQuaternion(1.0f, 0.0f, 0.0f, 0.0f);

		// Normalize only if not already unit length
		nVector3 axis = axisIn;
		if (fabsf(lenSq - 1.0f) > 1e-6f)
			axis = Normalized(axisIn);

		const float halfAngle = angleRad / 2.0f;
		const float s = sinf(halfAngle);

		return nQuaternion(cosf(halfAngle), axis.x * s, axis.y * s, axis.z * s);
	}

	nQuaternion from_AngularVelocity(const nVector3& omega, float dt)
	{
		const float lenSq = omega.x * omega.x + omega.y * omega.y + omega.z * omega.z;
		if (lenSq == 0.0f)
			return nQuaternion(1.0f, 0.0f, 0.0f, 0.0f);

		const float len = omega.Length();
		const nVector3 axis = omega / len;
		const float angle = len * dt;
		const float halfAngle = angle / 2.0f;
		const float s = sinf(halfAngle);

		return nQuaternion(cosf(halfAngle), axis.x * s, axis.y * s, axis.z * s);
	}

	nMatrix4 to_nMatrix4(const nQuaternion& quat) noexcept
	{
		const float w = quat.w;
		const float x = quat.x;
		const float y = quat.y;
		const float z = quat.z;

		const float xx = x * x;
		const float yy = y * y;
		const float zz = z * z;
		const float xy = x * y;
		const float xz = x * z;
		const float yz = y * z;
		const float wx = w * x;
		const float wy = w * y;
		const float wz = w * z;

		return nMatrix4{
			1.0f - 2.0f * (yy + zz), 2.0f * (xy + wz), 2.0f * (xz - wy), 0.0f,
			2.0f * (xy - wz), 1.0f - 2.0f * (xx + zz), 2.0f * (yz + wx), 0.0f,
			2.0f * (xz + wy), 2.0f * (yz - wx), 1.0f - 2.0f * (xx + yy), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	}

	nVector3 Vec_Rotate(const nQuaternion& quat, const nVector3& vec)
	{
		// v' = q * (0, v) * q^-1
		const nQuaternion inv = QInverse(quat);
		const nQuaternion pure(0.0f, vec.x, vec.y, vec.z);
		const nQuaternion result = quat * pure * inv;

		return nVector3(result.x, result.y, result.z);
	}

	nQuaternion QIntegrate(nQuaternion orientation, const nVector3& angularV, float dt)
	{
		const float lenSq = DotProduct(angularV, angularV);
		if (lenSq < 1e-8f)
			return orientation;

		// dq = 0.5 * orientation * (0, omega) * dt
		const nQuaternion dq = 0.5f * orientation * nQuaternion(0.0f, angularV.x, angularV.y, angularV.z) * dt;

		orientation = orientation + dq;

		return QNormalize(orientation);
	}

	nQuaternion QSlerp(const nQuaternion& a, const nQuaternion& b, float t)
	{
		const nQuaternion q1 = QNormalize(a);
		nQuaternion q2 = QNormalize(b);

		float dot = QDotProduct(a, b);

		// Take the short path
		if (dot < 0.0f)
		{
			q2 = nQuaternion(-q2.w, -q2.x, -q2.y, -q2.z);
			dot = -dot;
		}

		const float angle = acosf(Clamp(dot, -1.0f, 1.0f));

		// Angles very close: fall back to nlerp
		if (dot > 1.0f - EPSILON)
		{
			return QNormalize((1.0f - t) * q1 + t * q2);
		}

		const float s = sinf(angle);

		return (sinf((1.0f - t) * angle) / s) * a + (sinf(t * angle) / s) * b;
	}

	nQuaternion QNlerp(const nQuaternion& a, const nQuaternion& b, float t)
	{
		const nQuaternion q1 = QNormalize(a);
		nQuaternion q2 = QNormalize(b);

		// Take the short path
		if (QDotProduct(a, b) < 0.0f)
		{
			q2 = nQuaternion(-q2.w, -q2.x, -q2.y, -q2.z);
		}

		return QNormalize((1.0f - t) * q1 + t * q2);
	}

	nQuaternion from_EulerXYZ(float xRad, float yRad, float zRad)
	{
		const nQuaternion qx = from_AxisAngle(nVector3(1.0f, 0.0f, 0.0f), xRad);
		const nQuaternion qy = from_AxisAngle(nVector3(0.0f, 1.0f, 0.0f), yRad);
		const nQuaternion qz = from_AxisAngle(nVector3(0.0f, 0.0f, 1.0f), zRad);

		return qz * qy * qx;
	}

	nVector3 QuaternionToEuler(const nQuaternion& q)
	{
		// Pitch (X)
		const float sinPitch = 2.0f * (q.w * q.x + q.y * q.z);
		const float cosPitch = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
		const float pitch = atan2f(sinPitch, cosPitch);

		// Yaw (Y)
		const float sinYaw = 2.0f * (q.w * q.y - q.z * q.x);
		float yaw = 0.0f;
		if (fabsf(sinYaw) >= 1.0f)
			yaw = copysignf(3.14159265f / 2.0f, sinYaw);
		else
			yaw = asinf(sinYaw);

		// Roll (Z)
		const float sinRoll = 2.0f * (q.w * q.z + q.x * q.y);
		const float cosRoll = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
		const float roll = atan2f(sinRoll, cosRoll);

		return nVector3(pitch, yaw, roll);
	}
}
