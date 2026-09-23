#pragma once
#include <initializer_list>


namespace nNewton
{
//>>>========================== |  MATH UTILITIES | =============================<<<<///

	inline constexpr float PI = 3.1459456f;
	inline constexpr float DegToRad = PI / 180;
	inline constexpr float RadToDeg = 180 / PI;
	inline constexpr float SQRT2 = 1.4142135f;
	inline constexpr float EPSILON = 1e-6f;

	constexpr float Radians(float deg)
	{
		return deg * DegToRad;
	}

	constexpr float Degrees(float rad)
	{
		return rad * RadToDeg;
	}

	template<typename T>
	constexpr T Clamp(T v, T min, T max)
	{
		return v < min ? min : (v > max ? max : v);
	}

	template<typename T>
	constexpr T Lerp(const T& a, const T& b, float t)
	{
		return a + (b - a) * t;
	}

//====================== VECTOR 2 =======================//
	struct nVector2
	{
		float x, y;

		// -- Constructors --
		constexpr nVector2() noexcept;
		constexpr explicit nVector2(float scalar) noexcept;
		constexpr nVector2(float x_, float y_) noexcept;
		constexpr nVector2(const nVector2&) = default;
		constexpr nVector2(nVector2&&) noexcept = default;
		constexpr nVector2& operator=(const nVector2&) & = default;
		constexpr nVector2& operator=(nVector2&&) & noexcept = default;
		~nVector2() = default;

		// -- Arithmetic --
		constexpr nVector2 operator+(const nVector2& otr) const;
		constexpr nVector2 operator-(const nVector2& otr) const;
		constexpr nVector2 operator-() const noexcept;

		constexpr nVector2& operator+=(const nVector2& otr) noexcept;
		constexpr nVector2& operator-=(const nVector2& otr) noexcept;
		// component wise multi
		constexpr nVector2& operator*=(const nVector2& otr) noexcept;

		constexpr nVector2 operator*(float scalar) const noexcept;
		constexpr nVector2 operator/(float scalar) const;
		constexpr nVector2& operator*=(float scalar) noexcept;
		constexpr nVector2& operator/=(float scalar);

		// -- Comparison --
		constexpr bool operator==(const nVector2& otr) const noexcept;
		constexpr bool operator!=(const nVector2& otr) const noexcept;
		constexpr bool operator< (const nVector2& otr) const noexcept;
		constexpr bool operator<=(const nVector2& otr) const noexcept;
		constexpr bool operator> (const nVector2& otr) const noexcept;
		constexpr bool operator>=(const nVector2& otr) const noexcept;

		float Length() const noexcept;
	};

	constexpr nVector2 operator*(float scalar, const nVector2& vec) noexcept;

	constexpr float DotProduct(const nVector2& a, const nVector2& b) noexcept;
	constexpr float CrossProduct(const nVector2& a, const nVector2& b) noexcept;
	constexpr nVector2 Perpendicular(const nVector2& v) noexcept;

	nVector2 Normalized(const nVector2& a) noexcept;

//====================== VECTOR 3 =======================//
	struct nVector3
	{
		float x, y, z;

		// -- Constructors --
		constexpr nVector3() noexcept;
		constexpr explicit nVector3(float scalar) noexcept;
		constexpr nVector3(float x_, float y_, float z_) noexcept;
		constexpr nVector3(const nVector3&) = default;
		constexpr nVector3(nVector3&&) noexcept = default;
		constexpr nVector3& operator=(const nVector3&) & = default;
		constexpr nVector3& operator=(nVector3&&) & noexcept = default;
		~nVector3() = default;

		// -- Arithmetic --
		constexpr nVector3 operator+(const nVector3& otr) const;
		constexpr nVector3 operator-(const nVector3& otr) const;
		constexpr nVector3 operator-() const noexcept;
		// component wise mult
		constexpr nVector3 operator*(const nVector3& otr) const noexcept;

		constexpr nVector3& operator+=(const nVector3& otr) noexcept;
		constexpr nVector3& operator-=(const nVector3& otr) noexcept;
		constexpr nVector3& operator*=(const nVector3& otr) noexcept;

		constexpr nVector3 operator*(float scalar) const noexcept;
		constexpr nVector3 operator/(float scalar) const;
		constexpr nVector3& operator*=(float scalar) noexcept;
		constexpr nVector3& operator/=(float scalar);

		// -- Comparison --
		constexpr bool operator==(const nVector3& otr) const noexcept;
		constexpr bool operator!=(const nVector3& otr) const noexcept;
		constexpr bool operator< (const nVector3& otr) const noexcept;
		constexpr bool operator<=(const nVector3& otr) const noexcept;
		constexpr bool operator> (const nVector3& otr) const noexcept;
		constexpr bool operator>=(const nVector3& otr) const noexcept;

		float Length() const noexcept;
	};

	constexpr nVector3 operator*(float scalar, const nVector3& vec) noexcept;
	constexpr float DotProduct(const nVector3& a, const nVector3& b) noexcept;
	constexpr nVector3 CrossProduct(const nVector3& a, const nVector3& b) noexcept;
	nVector3 Normalized(const nVector3& a) noexcept;
	nVector3 Min(const nVector3& a, const nVector3& b) noexcept;
	nVector3 Max(const nVector3& a, const nVector3& b) noexcept;

//======================VECTOR 4=======================//

	struct nVector4
	{
		float x, y, z, w;

		// -- Constructors --
		constexpr nVector4() noexcept;
		constexpr explicit nVector4(float scalar) noexcept;
		constexpr nVector4(float x_, float y_, float z_, float w_) noexcept;
		constexpr nVector4(const nVector4&) = default;
		constexpr nVector4(nVector4&&) noexcept = default;
		constexpr nVector4& operator=(const nVector4&) & = default;
		constexpr nVector4& operator=(nVector4&&) & noexcept = default;
		~nVector4() = default;

		// -- Arithmetic --
		constexpr nVector4 operator+(const nVector4& otr) const;
		constexpr nVector4 operator-(const nVector4& otr) const;
		constexpr nVector4 operator-() const noexcept;

		constexpr nVector4& operator+=(const nVector4& otr) noexcept;
		constexpr nVector4& operator-=(const nVector4& otr) noexcept;
		constexpr nVector4& operator*=(const nVector4& otr) noexcept;

		constexpr nVector4 operator*(float scalar) const noexcept;
		constexpr nVector4 operator/(float scalar) const;
		constexpr nVector4& operator*=(float scalar) noexcept;
		constexpr nVector4& operator/=(float scalar);

		// -- Comparison --
		constexpr bool operator==(const nVector4& otr) const noexcept;
		constexpr bool operator!=(const nVector4& otr) const noexcept;
		constexpr bool operator< (const nVector4& otr) const noexcept;
		constexpr bool operator<=(const nVector4& otr) const noexcept;
		constexpr bool operator> (const nVector4& otr) const noexcept;
		constexpr bool operator>=(const nVector4& otr) const noexcept;

		float Length() const noexcept;
	};

	constexpr nVector4 operator*(float scalar, const nVector4& vec) noexcept;
	constexpr float DotProduct(const nVector4& a, const nVector4& b) noexcept;
	constexpr nVector4 CrossProduct(const nVector4& a, const nVector4& b) noexcept;
	nVector4 Normalized(const nVector4& a) noexcept;

//========>>MAT 3X3<<======//

	struct nMatrix3
	{
		float A[9];

		// -- Constructors --
		constexpr nMatrix3() noexcept;
		constexpr explicit nMatrix3(float scalar) noexcept;
		constexpr nMatrix3(std::initializer_list<float> values);
		constexpr nMatrix3(const nMatrix3&) = default;
		constexpr nMatrix3(nMatrix3&&) noexcept = default;
		constexpr nMatrix3& operator=(const nMatrix3&) & = default;
		constexpr nMatrix3& operator=(nMatrix3&&) & noexcept = default;
		~nMatrix3() = default;

		nVector3 operator*(const nVector3& v) const;

		constexpr nMatrix3 operator*(float scalar) const noexcept;
		friend constexpr nMatrix3 operator*(float scalar, const nMatrix3& m) noexcept;
		constexpr nMatrix3 Inverse() const;
	};

	constexpr nMatrix3 Identity3() noexcept;

//++============================ MATRIX 4X4 ===================================++//
//
// Column Major: A[col * 4 + row]

	struct nMatrix4
	{
		float A[16];

		// -- Constructors --
		constexpr nMatrix4() noexcept;
		constexpr explicit nMatrix4(float scalar) noexcept;
		constexpr nMatrix4(std::initializer_list<float> values);
		constexpr nMatrix4(const nMatrix4&) = default;
		constexpr nMatrix4(nMatrix4&&) noexcept = default;
		constexpr nMatrix4& operator=(const nMatrix4&) & = default;
		constexpr nMatrix4& operator=(nMatrix4&&) & noexcept = default;
		~nMatrix4() = default;

		nMatrix4 operator+(const nMatrix4& otr) const;
		nMatrix4 operator-(const nMatrix4& otr) const;
		nMatrix4 operator*(const nMatrix4& otr) const;
		nVector4 operator*(const nVector4& v) const;

		nMatrix4 Inverse() const;
		constexpr nMatrix4 operator*(float scalar) const noexcept;
		friend constexpr nMatrix4 operator*(float scalar, const nMatrix4& m) noexcept;
	};

	constexpr nMatrix4 Identity4() noexcept;
	float Determinant(const nMatrix4& m) noexcept;

	nMatrix4 RotateX(float rad);
	nMatrix4 RotateY(float rad);
	nMatrix4 RotateZ(float rad);
	nMatrix4 Translate(const nVector3& t);
	nMatrix4 Scale(const nVector3& s);

	nMatrix4 Transpose(const nMatrix4& m) noexcept;

	nMatrix4 Rotate(float rad, const nVector3& axis);

	nMatrix4 Look_At(const nVector3& eye, const nVector3& center, const nVector3& up);
	nMatrix4 Perspective(float FOV, float aspect, float nearZ, float farZ);
	nMatrix4 Ortho(float left, float right, float bottom, float top, float nearZ, float farZ);

//++============================ QUATERNION ===================================++//

	struct nQuaternion
	{
		float w, x, y, z;

		// -- Constructors --
		constexpr nQuaternion() noexcept;
		constexpr explicit nQuaternion(float w_) noexcept;
		constexpr nQuaternion(float w_, float x_, float y_, float z_) noexcept;
		constexpr nQuaternion(const nQuaternion&) = default;
		constexpr nQuaternion(nQuaternion&&) noexcept = default;
		constexpr nQuaternion& operator=(const nQuaternion&) & = default;
		constexpr nQuaternion& operator=(nQuaternion&&) & noexcept = default;
		~nQuaternion() = default;

		// -- Arithmetic --
		constexpr nQuaternion operator+(const nQuaternion& otr) const;
		constexpr nQuaternion operator-(const nQuaternion& otr) const;
		constexpr nQuaternion operator*(const nQuaternion& otr) const;
		constexpr nQuaternion operator*(float scalar) const noexcept;

		float Length() const noexcept;
	};

	nQuaternion operator*(float scalar, const nQuaternion& quat) noexcept;

	nQuaternion QNormalize(const nQuaternion& quat) noexcept;
	nQuaternion Conjugate(const nQuaternion& quat) noexcept;
	nQuaternion QInverse(const nQuaternion& quat) noexcept;

	float QDotProduct(const nQuaternion& a, const nQuaternion& b) noexcept;

	nQuaternion from_AxisAngle(const nVector3& axis, float angleRad);
	nQuaternion from_AngularVelocity(const nVector3& omega, float dt);

	nMatrix4 to_nMatrix4(const nQuaternion& quat) noexcept;
	nVector3 Vec_Rotate(const nQuaternion& quat, const nVector3& vec);

	nQuaternion QIntegrate(nQuaternion orientation, const nVector3& angularV, float dt);
	nQuaternion QSlerp(const nQuaternion& a, const nQuaternion& b, float t);
	nQuaternion QNlerp(const nQuaternion& a, const nQuaternion& b, float t);
	nQuaternion from_EulerXYZ(float xRad, float yRad, float zRad);
	nVector3 QuaternionToEuler(const nQuaternion& q);
}

#include "nMath.inl"
