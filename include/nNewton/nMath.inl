#pragma once 
#include <cassert>
namespace nNewton
{
	//VEC2
	//++++++========++++

	constexpr nVector2::nVector2() : x(0), y(0)
	{
	}
	constexpr nVector2::nVector2(float m) : x(m), y(m)
	{
	}
	constexpr nVector2::nVector2(float m_, float c_) : x(m_), y(c_)
	{
	}

	//>>============= OPR
	constexpr nVector2 nVector2::operator+(const nVector2& otr)const
	{
		return nVector2(this->x + otr.x, this->y + otr.y);
	}
	constexpr nVector2 nVector2::operator-(const nVector2& otr)const
	{
		return nVector2(this->x - otr.x, this->y - otr.y);
	}

	constexpr nVector2 nVector2::operator-() const
	{
		return nVector2(-(this->x), -(this->y));
	}

	constexpr nVector2& nVector2::operator+=(const nVector2& otr) {
		this->x = this->x + otr.x;
		this->y = this->y + otr.y;
		return  *this;
	}
	constexpr nVector2& nVector2::operator-=(const nVector2& otr) {
		this->x = this->x - otr.x;
		this->y = this->y - otr.y;
		return  *this;
	}

	// component wise multi
	constexpr nVector2& nVector2::operator*=(const nVector2& otr) {
		this->x = this->x * otr.x;
		this->y = this->y * otr.y;
		return  *this;
	}

	//>>============== vector scalar opr
	constexpr nVector2 nVector2::operator*(float scalar) const {
		return  nVector2(x * scalar, y * scalar);
	}

	constexpr nVector2 nVector2::operator/(float scalar) const {

		assert(scalar != 0.0f);
		return nVector2(x / scalar, y / scalar);
	}


	constexpr nVector2& nVector2::operator*=(float scalar) {
		this->x = this->x * scalar;
		this->y = this->y * scalar;
		return  *this;
	}
	constexpr nVector2& nVector2::operator/=(float scalar) {
		assert(scalar != 0.0f);
		x /= scalar;
		y /= scalar;
		return *this;
	}

	constexpr nVector2 operator*(float scalar, const nVector2& vec) {
		return { vec.x * scalar , vec.y * scalar };
	}

	constexpr float DotProduct(const nVector2& b, const nVector2& c) {
		return (b.x * c.x + b.y * c.y);
	}
	constexpr float CrossProduct(const nVector2& a, const nVector2& b) {
		return a.x * b.y - a.y * b.x;
	}
	constexpr nVector2 Perpendicular(const nVector2& v) {
		return nVector2(v.y, -v.x);
	}


	//==========  VECTOR 3 

	///CTOR
	constexpr nVector3::nVector3() : x(0), y(0), z(0)
	{
	}
	constexpr nVector3::nVector3(float m) : x(m), y(m), z(0)
	{
	}
	constexpr nVector3::nVector3(float m_, float k_, float c_) : x(m_), y(k_), z(c_)
	{
	}

	//>>============= OPR
	constexpr nVector3 nVector3::operator+(const nVector3& otr)const
	{
		return nVector3(this->x + otr.x, this->y + otr.y, this->z + otr.z);
	}
	constexpr nVector3 nVector3::operator-(const nVector3& otr)const
	{
		return nVector3(this->x - otr.x, this->y - otr.y, this->z - otr.z);
	}

	constexpr nVector3 nVector3::operator-() const
	{
		return nVector3(-(this->x), -(this->y), -(this->z));
	}


	//comp wise mult
	constexpr nVector3 nVector3::operator*(const nVector3& otr) const
	{
		return {
			x * otr.x,
			y * otr.y,
			z * otr.z
		};
	}
	constexpr nVector3& nVector3::operator+=(const nVector3& otr) {
		this->x = this->x + otr.x;
		this->y = this->y + otr.y;
		this->z = this->z + otr.z;
		return  *this;
	}
	constexpr nVector3& nVector3::operator-=(const nVector3& otr) {
		this->x = this->x - otr.x;
		this->y = this->y - otr.y;
		this->z = this->z - otr.z;
		return  *this;
	}

	// component wise multi
	constexpr nVector3& nVector3::operator*=(const nVector3& otr) {
		this->x = this->x * otr.x;
		this->y = this->y * otr.y;
		this->z = this->z * otr.z;
		return  *this;
	}

	//>>============== vector scalar opr
	constexpr nVector3 nVector3::operator*(float scalar) const {
		return  nVector3(x * scalar, y * scalar, z * scalar);
	}

	constexpr nVector3 nVector3::operator/(float scalar) const {

		assert(scalar != 0.0f);
		return nVector3(x / scalar, y / scalar, z / scalar);
	}


	constexpr nVector3& nVector3::operator*=(float scalar) {
		this->x = this->x * scalar;
		this->y = this->y * scalar;
		this->z = this->z * scalar;
		return  *this;
	}

	constexpr nVector3& nVector3::operator/=(float scalar) {
		assert(scalar != 0.0f);
		x /= scalar;
		y /= scalar;
		z /= scalar;
		return *this;
	}

	constexpr nVector3 operator*(float scalar, const nVector3& vec) {
		return { vec.x * scalar , vec.y * scalar, vec.z * scalar };
	}

	constexpr float DotProduct(const nVector3& b, const nVector3& c) {
		return (
			b.x * c.x +
			b.y * c.y +
			b.z * c.z
			);
	}
	constexpr nVector3 CrossProduct(const nVector3& a, const nVector3& b) {
		return  nVector3(
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		);
	}

	//======================VECTOR 4=======================//

	///CTOR
	constexpr nVector4::nVector4() : x(0), y(0), z(0), w(0)
	{
	}
	constexpr nVector4::nVector4(float m) : x(m), y(m), z(0), w(m)
	{
	}
	constexpr nVector4::nVector4(float b_, float s_, float d_, float k_) : x(b_), y(s_), z(d_), w(k_)
	{
	}

	//>>============= OPR
	constexpr nVector4 nVector4::operator+(const nVector4& otr)const
	{
		return nVector4(this->x + otr.x, this->y + otr.y, this->z + otr.z, this->w + otr.w);
	}
	constexpr nVector4 nVector4::operator-(const nVector4& otr)const
	{
		return nVector4(this->x - otr.x, this->y - otr.y, this->z - otr.z, this->w - otr.w);
	}

	constexpr nVector4 nVector4::operator-() const
	{
		return nVector4(-(this->x), -(this->y), -(this->z), -(this->w));
	}

	constexpr nVector4& nVector4::operator+=(const nVector4& otr) {
		this->x = this->x + otr.x;
		this->y = this->y + otr.y;
		this->z = this->z + otr.z;
		this->w = this->w + otr.w;
		return  *this;
	}
	constexpr nVector4& nVector4::operator-=(const nVector4& otr) {
		this->x = this->x - otr.x;
		this->y = this->y - otr.y;
		this->z = this->z - otr.z;
		this->w = this->w - otr.w;
		return  *this;
	}

	// component wise multi
	constexpr nVector4& nVector4::operator*=(const nVector4& otr) {
		this->x = this->x * otr.x;
		this->y = this->y * otr.y;
		this->z = this->z * otr.z;
		this->w = this->w * otr.w;
		return  *this;
	}

	//>>============== vector scalar opr
	constexpr nVector4 nVector4::operator*(float scalar) const {
		return  nVector4(x * scalar, y * scalar, z * scalar, w * scalar);
	}

	constexpr nVector4 nVector4::operator/(float scalar) const {

		assert(scalar != 0.0f);
		return nVector4(x / scalar, y / scalar, z / scalar, w / scalar);
	}


	constexpr nVector4& nVector4::operator*=(float scalar) {
		this->x = this->x * scalar;
		this->y = this->y * scalar;
		this->z = this->z * scalar;
		this->w = this->w * scalar;
		return  *this;
	}

	constexpr nVector4& nVector4::operator/=(float scalar) {
		assert(scalar != 0.0f);
		x /= scalar;
		y /= scalar;
		z /= scalar;
		w /= scalar;
		return *this;
	}

	constexpr nVector4 operator*(float scalar, const nVector4& vec) {
		return { vec.x * scalar , vec.y * scalar, vec.z * scalar ,vec.w * scalar };
	}

	constexpr float DotProduct(const nVector4& b, const nVector4& c) {
		return (
			b.x * c.x +
			b.y * c.y +
			b.z * c.z +
			b.w * c.w
			);
	}
	constexpr nVector4 CrossProduct(const nVector4& a, const nVector4& b) {
		return  nVector4(
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x,
			0.0f
		);
	}


	//=====    MATRIX 4 

	constexpr nMatrix4::nMatrix4()
	{
		for (int i = 0; i < 16; ++i)
		{
			A[i] = 0;
		}
	}
	constexpr nMatrix4::nMatrix4(float m)
	{
		for (int i = 0; i < 16; ++i)
		{
			A[i] = m;
		}
	}
	constexpr nMatrix4::nMatrix4(std::initializer_list<float> values)
	{
		assert(values.size() == 16);
		int i = 0;
		for (float v : values)
			A[i++] = v;
	}
	constexpr nMatrix4 Identity()
	{
		return nMatrix4{

			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	}

	// Quaternions ===========>>>>>>>>>>>

	constexpr nQuaternion::nQuaternion() : w(1), x(0), y(0), z(0) {}
	constexpr nQuaternion::nQuaternion(float w_) : w(w_), x(0), y(0), z(0) {}
	constexpr nQuaternion::nQuaternion(float w_, float x_, float y_, float z_) : w(w_), x(x_), y(y_), z(z_) {}

	constexpr nQuaternion nQuaternion::operator+(const nQuaternion& otr)const
	{
		return nQuaternion(this->w + otr.w, this->x + otr.x, this->y + otr.y, this->z + otr.z);
	}

	constexpr nQuaternion nQuaternion::operator-(const nQuaternion& otr)const
	{
		return nQuaternion(this->w - otr.w, this->x - otr.x, this->y - otr.y, this->z - otr.z);
	}

	constexpr nQuaternion nQuaternion::operator*(const nQuaternion& otr)const
	{
		return {
			w * otr.w - x * otr.x - y * otr.y - z * otr.z,
			w * otr.x + x * otr.w + y * otr.z - z * otr.y,
			w * otr.y - x * otr.z + y * otr.w + z * otr.x,
			w * otr.z + x * otr.y - y * otr.x + z * otr.w
		};
	}

	constexpr nQuaternion nQuaternion::operator*(float scalar_) const
	{
		return { scalar_ * w, scalar_ * x, scalar_ * y, scalar_ * z };
	}
}