#pragma once
#include <cassert>
namespace nNewton
{
//====================== VECTOR 2 =======================//

	constexpr nVector2::nVector2() noexcept : x(0.0f), y(0.0f)
	{
	}
	constexpr nVector2::nVector2(float scalar) noexcept : x(scalar), y(scalar)
	{
	}
	constexpr nVector2::nVector2(float x_, float y_) noexcept : x(x_), y(y_)
	{
	}

	// -- Operators --
	constexpr nVector2 nVector2::operator+(const nVector2& otr) const
	{
		return nVector2(x + otr.x, y + otr.y);
	}
	constexpr nVector2 nVector2::operator-(const nVector2& otr) const
	{
		return nVector2(x - otr.x, y - otr.y);
	}
	constexpr nVector2 nVector2::operator-() const noexcept
	{
		return nVector2(-x, -y);
	}

	constexpr nVector2& nVector2::operator+=(const nVector2& otr) noexcept
	{
		x += otr.x;
		y += otr.y;
		return *this;
	}
	constexpr nVector2& nVector2::operator-=(const nVector2& otr) noexcept
	{
		x -= otr.x;
		y -= otr.y;
		return *this;
	}
	// component wise multi
	constexpr nVector2& nVector2::operator*=(const nVector2& otr) noexcept
	{
		x *= otr.x;
		y *= otr.y;
		return *this;
	}

	// -- Scalar operators --
	constexpr nVector2 nVector2::operator*(float scalar) const noexcept
	{
		return nVector2(x * scalar, y * scalar);
	}
	constexpr nVector2 nVector2::operator/(float scalar) const
	{
		assert(scalar != 0.0f);
		return nVector2(x / scalar, y / scalar);
	}
	constexpr nVector2& nVector2::operator*=(float scalar) noexcept
	{
		x *= scalar;
		y *= scalar;
		return *this;
	}
	constexpr nVector2& nVector2::operator/=(float scalar)
	{
		assert(scalar != 0.0f);
		x /= scalar;
		y /= scalar;
		return *this;
	}

	constexpr nVector2 operator*(float scalar, const nVector2& vec) noexcept
	{
		return nVector2(vec.x * scalar, vec.y * scalar);
	}

	constexpr float DotProduct(const nVector2& a, const nVector2& b) noexcept
	{
		return a.x * b.x + a.y * b.y;
	}
	constexpr float CrossProduct(const nVector2& a, const nVector2& b) noexcept
	{
		return a.x * b.y - a.y * b.x;
	}
	constexpr nVector2 Perpendicular(const nVector2& v) noexcept
	{
		return nVector2(v.y, -v.x);
	}

	// -- Comparison --
	constexpr bool nVector2::operator==(const nVector2& otr) const noexcept
	{
		return x == otr.x && y == otr.y;
	}
	constexpr bool nVector2::operator!=(const nVector2& otr) const noexcept
	{
		return !(*this == otr);
	}
	constexpr bool nVector2::operator<(const nVector2& otr) const noexcept
	{
		if (x != otr.x) return x < otr.x;
		return y < otr.y;
	}
	constexpr bool nVector2::operator>(const nVector2& otr) const noexcept
	{
		return otr < *this;
	}
	constexpr bool nVector2::operator<=(const nVector2& otr) const noexcept
	{
		return !(otr < *this);
	}
	constexpr bool nVector2::operator>=(const nVector2& otr) const noexcept
	{
		return !(*this < otr);
	}

//====================== VECTOR 3 =======================//

	constexpr nVector3::nVector3() noexcept : x(0.0f), y(0.0f), z(0.0f)
	{
	}
	constexpr nVector3::nVector3(float scalar) noexcept : x(scalar), y(scalar), z(scalar)
	{
	}
	constexpr nVector3::nVector3(float x_, float y_, float z_) noexcept : x(x_), y(y_), z(z_)
	{
	}

	// -- Operators --
	constexpr nVector3 nVector3::operator+(const nVector3& otr) const
	{
		return nVector3(x + otr.x, y + otr.y, z + otr.z);
	}
	constexpr nVector3 nVector3::operator-(const nVector3& otr) const
	{
		return nVector3(x - otr.x, y - otr.y, z - otr.z);
	}
	constexpr nVector3 nVector3::operator-() const noexcept
	{
		return nVector3(-x, -y, -z);
	}

	// component wise mult
	constexpr nVector3 nVector3::operator*(const nVector3& otr) const noexcept
	{
		return nVector3(x * otr.x, y * otr.y, z * otr.z);
	}

	constexpr nVector3& nVector3::operator+=(const nVector3& otr) noexcept
	{
		x += otr.x;
		y += otr.y;
		z += otr.z;
		return *this;
	}
	constexpr nVector3& nVector3::operator-=(const nVector3& otr) noexcept
	{
		x -= otr.x;
		y -= otr.y;
		z -= otr.z;
		return *this;
	}
	// component wise multi
	constexpr nVector3& nVector3::operator*=(const nVector3& otr) noexcept
	{
		x *= otr.x;
		y *= otr.y;
		z *= otr.z;
		return *this;
	}

	// -- Scalar operators --
	constexpr nVector3 nVector3::operator*(float scalar) const noexcept
	{
		return nVector3(x * scalar, y * scalar, z * scalar);
	}
	constexpr nVector3 nVector3::operator/(float scalar) const
	{
		assert(scalar != 0.0f);
		return nVector3(x / scalar, y / scalar, z / scalar);
	}
	constexpr nVector3& nVector3::operator*=(float scalar) noexcept
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}
	constexpr nVector3& nVector3::operator/=(float scalar)
	{
		assert(scalar != 0.0f);
		x /= scalar;
		y /= scalar;
		z /= scalar;
		return *this;
	}

	constexpr nVector3 operator*(float scalar, const nVector3& vec) noexcept
	{
		return nVector3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
	}

	constexpr float DotProduct(const nVector3& a, const nVector3& b) noexcept
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}
	constexpr nVector3 CrossProduct(const nVector3& a, const nVector3& b) noexcept
	{
		return nVector3(
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		);
	}

	// -- Comparison --
	constexpr bool nVector3::operator==(const nVector3& otr) const noexcept
	{
		return x == otr.x && y == otr.y && z == otr.z;
	}
	constexpr bool nVector3::operator!=(const nVector3& otr) const noexcept
	{
		return !(*this == otr);
	}
	constexpr bool nVector3::operator<(const nVector3& otr) const noexcept
	{
		if (x != otr.x) return x < otr.x;
		if (y != otr.y) return y < otr.y;
		return z < otr.z;
	}
	constexpr bool nVector3::operator>(const nVector3& otr) const noexcept
	{
		return otr < *this;
	}
	constexpr bool nVector3::operator<=(const nVector3& otr) const noexcept
	{
		return !(otr < *this);
	}
	constexpr bool nVector3::operator>=(const nVector3& otr) const noexcept
	{
		return !(*this < otr);
	}

//====================== VECTOR 4 =======================//

	constexpr nVector4::nVector4() noexcept : x(0.0f), y(0.0f), z(0.0f), w(0.0f)
	{
	}
	constexpr nVector4::nVector4(float scalar) noexcept : x(scalar), y(scalar), z(scalar), w(scalar)
	{
	}
	constexpr nVector4::nVector4(float x_, float y_, float z_, float w_) noexcept : x(x_), y(y_), z(z_), w(w_)
	{
	}

	// -- Operators --
	constexpr nVector4 nVector4::operator+(const nVector4& otr) const
	{
		return nVector4(x + otr.x, y + otr.y, z + otr.z, w + otr.w);
	}
	constexpr nVector4 nVector4::operator-(const nVector4& otr) const
	{
		return nVector4(x - otr.x, y - otr.y, z - otr.z, w - otr.w);
	}
	constexpr nVector4 nVector4::operator-() const noexcept
	{
		return nVector4(-x, -y, -z, -w);
	}

	constexpr nVector4& nVector4::operator+=(const nVector4& otr) noexcept
	{
		x += otr.x;
		y += otr.y;
		z += otr.z;
		w += otr.w;
		return *this;
	}
	constexpr nVector4& nVector4::operator-=(const nVector4& otr) noexcept
	{
		x -= otr.x;
		y -= otr.y;
		z -= otr.z;
		w -= otr.w;
		return *this;
	}
	// component wise multi
	constexpr nVector4& nVector4::operator*=(const nVector4& otr) noexcept
	{
		x *= otr.x;
		y *= otr.y;
		z *= otr.z;
		w *= otr.w;
		return *this;
	}

	// -- Scalar operators --
	constexpr nVector4 nVector4::operator*(float scalar) const noexcept
	{
		return nVector4(x * scalar, y * scalar, z * scalar, w * scalar);
	}
	constexpr nVector4 nVector4::operator/(float scalar) const
	{
		assert(scalar != 0.0f);
		return nVector4(x / scalar, y / scalar, z / scalar, w / scalar);
	}
	constexpr nVector4& nVector4::operator*=(float scalar) noexcept
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		w *= scalar;
		return *this;
	}
	constexpr nVector4& nVector4::operator/=(float scalar)
	{
		assert(scalar != 0.0f);
		x /= scalar;
		y /= scalar;
		z /= scalar;
		w /= scalar;
		return *this;
	}

	constexpr nVector4 operator*(float scalar, const nVector4& vec) noexcept
	{
		return nVector4(vec.x * scalar, vec.y * scalar, vec.z * scalar, vec.w * scalar);
	}

	constexpr float DotProduct(const nVector4& a, const nVector4& b) noexcept
	{
		return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	}
	constexpr nVector4 CrossProduct(const nVector4& a, const nVector4& b) noexcept
	{
		return nVector4(
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x,
			0.0f
		);
	}

	// -- Comparison --
	constexpr bool nVector4::operator==(const nVector4& otr) const noexcept
	{
		return x == otr.x && y == otr.y && z == otr.z && w == otr.w;
	}
	constexpr bool nVector4::operator!=(const nVector4& otr) const noexcept
	{
		return !(*this == otr);
	}
	constexpr bool nVector4::operator<(const nVector4& otr) const noexcept
	{
		if (x != otr.x) return x < otr.x;
		if (y != otr.y) return y < otr.y;
		if (z != otr.z) return z < otr.z;
		return w < otr.w;
	}
	constexpr bool nVector4::operator>(const nVector4& otr) const noexcept
	{
		return otr < *this;
	}
	constexpr bool nVector4::operator<=(const nVector4& otr) const noexcept
	{
		return !(otr < *this);
	}
	constexpr bool nVector4::operator>=(const nVector4& otr) const noexcept
	{
		return !(*this < otr);
	}

//====================== MATRIX 3 =======================//

	constexpr nMatrix3::nMatrix3() noexcept
	{
		for (int i = 0; i < 9; ++i)
			A[i] = 0.0f;
	}
	constexpr nMatrix3::nMatrix3(float scalar) noexcept
	{
		for (int i = 0; i < 9; ++i)
			A[i] = scalar;
	}
	constexpr nMatrix3::nMatrix3(std::initializer_list<float> values)
	{
		assert(values.size() == 9);
		int i = 0;
		for (float v : values)
			A[i++] = v;
	}

	constexpr nMatrix3 nMatrix3::operator*(float scalar) const noexcept
	{
		nMatrix3 result;
		for (int i = 0; i < 9; ++i)
			result.A[i] = A[i] * scalar;
		return result;
	}

	constexpr nMatrix3 operator*(float scalar, const nMatrix3& m) noexcept
	{
		return m * scalar;
	}

	constexpr nMatrix3 nMatrix3::Inverse() const
	{
		const float m00 = A[0], m01 = A[3], m02 = A[6];
		const float m10 = A[1], m11 = A[4], m12 = A[7];
		const float m20 = A[2], m21 = A[5], m22 = A[8];

		// Cofactors of the first row
		const float c00 = m11 * m22 - m12 * m21;
		const float c01 = -m10 * m22 + m12 * m20;
		const float c02 = m10 * m21 - m11 * m20;

		const float c10 = -m01 * m22 + m02 * m21;
		const float c11 = m00 * m22 - m02 * m20;
		const float c12 = -m00 * m21 + m01 * m20;

		const float c20 = m01 * m12 - m02 * m11;
		const float c21 = -m00 * m12 + m02 * m10;
		const float c22 = m00 * m11 - m01 * m10;

		const float det = m00 * c00 + m01 * c01 + m02 * c02;
		if (det == 0.0f)
			return Identity3();

		const float invDet = 1.0f / det;

		nMatrix3 result;
		result.A[0] = c00 * invDet;
		result.A[1] = c01 * invDet;
		result.A[2] = c02 * invDet;
		result.A[3] = c10 * invDet;
		result.A[4] = c11 * invDet;
		result.A[5] = c12 * invDet;
		result.A[6] = c20 * invDet;
		result.A[7] = c21 * invDet;
		result.A[8] = c22 * invDet;
		return result;
	}

	constexpr nMatrix3 Identity3() noexcept
	{
		return nMatrix3{
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f
		};
	}

//====================== MATRIX 4 =======================//

	constexpr nMatrix4::nMatrix4() noexcept
	{
		for (int i = 0; i < 16; ++i)
			A[i] = 0.0f;
	}
	constexpr nMatrix4::nMatrix4(float scalar) noexcept
	{
		for (int i = 0; i < 16; ++i)
			A[i] = scalar;
	}
	constexpr nMatrix4::nMatrix4(std::initializer_list<float> values)
	{
		assert(values.size() == 16);
		int i = 0;
		for (float v : values)
			A[i++] = v;
	}

	constexpr nMatrix4 Identity4() noexcept
	{
		return nMatrix4{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	}

	constexpr nMatrix4 nMatrix4::operator*(float scalar) const noexcept
	{
		nMatrix4 result;
		for (int i = 0; i < 16; ++i)
			result.A[i] = A[i] * scalar;
		return result;
	}

	constexpr nMatrix4 operator*(float scalar, const nMatrix4& m) noexcept
	{
		return m * scalar;
	}

//====================== QUATERNION =======================//

	constexpr nQuaternion::nQuaternion() noexcept : w(1.0f), x(0.0f), y(0.0f), z(0.0f)
	{
	}
	constexpr nQuaternion::nQuaternion(float w_) noexcept : w(w_), x(0.0f), y(0.0f), z(0.0f)
	{
	}
	constexpr nQuaternion::nQuaternion(float w_, float x_, float y_, float z_) noexcept
		: w(w_), x(x_), y(y_), z(z_)
	{
	}

	// -- Operators --
	constexpr nQuaternion nQuaternion::operator+(const nQuaternion& otr) const
	{
		return nQuaternion(w + otr.w, x + otr.x, y + otr.y, z + otr.z);
	}

	constexpr nQuaternion nQuaternion::operator-(const nQuaternion& otr) const
	{
		return nQuaternion(w - otr.w, x - otr.x, y - otr.y, z - otr.z);
	}

	constexpr nQuaternion nQuaternion::operator*(const nQuaternion& otr) const
	{
		// Hamilton product
		return nQuaternion(
			w * otr.w - x * otr.x - y * otr.y - z * otr.z,
			w * otr.x + x * otr.w + y * otr.z - z * otr.y,
			w * otr.y - x * otr.z + y * otr.w + z * otr.x,
			w * otr.z + x * otr.y - y * otr.x + z * otr.w
		);
	}

	constexpr nQuaternion nQuaternion::operator*(float scalar) const noexcept
	{
		return nQuaternion(scalar * w, scalar * x, scalar * y, scalar * z);
	}
}
