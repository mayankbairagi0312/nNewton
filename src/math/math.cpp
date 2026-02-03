#include <nNewton/math.hpp>
#include<glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cassert>
#include <cmath>

namespace nNewton
{
//VECTOR 2
	
	///CTOR
	constexpr nVector2::nVector2(): x(0),y(0)
	{ }
	constexpr nVector2::nVector2(float m) :x(m),y(m)
	{ }
	constexpr nVector2::nVector2(float m_, float c_): x(m_),y(c_)
	{ }

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
		return nVector2(-(this->x),-( this->y) );
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
		return  nVector2(x*scalar,y*scalar);
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

	float nVector2::Length()const
	{
		return sqrt(x * x + y * y);
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
		return nVector2(v.y ,-v.x);
	}

	nVector2 Normalized(const nVector2& a) {
		float len = a.Length();
		if (len == 0.0f)
			return nVector2(0.0f);
		return a / len;
	}




	//==========  VECTOR 3 

	///CTOR
	constexpr nVector3::nVector3() : x(0), y(0),z(0)
	{
	}
	constexpr nVector3::nVector3(float m) :x(m), y(m),z(0)
	{
	}
	constexpr nVector3::nVector3(float m_,float k_, float c_) : x(m_), y(k_),z(c_)
	{
	}

	//>>============= OPR
	constexpr nVector3 nVector3::operator+(const nVector3& otr)const
	{
		return nVector3(this->x + otr.x, this->y + otr.y,this->z +otr.z);
	}
	constexpr nVector3 nVector3::operator-(const nVector3& otr)const
	{
		return nVector3(this->x - otr.x, this->y - otr.y, this->z - otr.z);
	}

	constexpr nVector3 nVector3::operator-() const
	{
		return nVector3(-(this->x), -(this->y), -(this->z));
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
		return  nVector3(x * scalar, y * scalar,z*scalar);
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

	float nVector3::Length()const
	{
		return sqrt(x * x + y * y + z*z);
	}

	constexpr nVector3 operator*(float scalar, const nVector3& vec) {
		return { vec.x * scalar , vec.y * scalar, vec.z*scalar };
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


	nVector3 Normalized(const nVector3& a) {
		float len = a.Length();
		if (len == 0.0f)
			return nVector3(0.0f);
		return a / len;
	}


	//======================VECTOR 4=======================//

	///CTOR
	constexpr nVector4::nVector4() : x(0), y(0), z(0),w(0)
	{
	}
	constexpr nVector4::nVector4(float m) :x(m), y(m), z(0),w(m)
	{
	}
	constexpr nVector4::nVector4(float b_, float s_, float d_, float k_) : x(b_), y(s_), z(d_),w(k_)
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
		return  nVector4(x * scalar, y * scalar, z * scalar,w*scalar);
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

	float nVector4::Length()const
	{
		return sqrt(x * x + y * y + z * z + w * w);
	}

	constexpr nVector4 operator*(float scalar, const nVector4& vec) {
		return { vec.x * scalar , vec.y * scalar, vec.z * scalar ,vec.w*scalar};
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


	nVector4 Normalized(const nVector4& a) {
		float len = a.Length();
		if (len == 0.0f)
			return nVector4(0.0f);
		return a / len;
	}



	//=====    MATRIX 4 

	constexpr nMatrix4::nMatrix4() 
	{ 
		for (int i = 0; i < 16 ;++i)
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
	


	nMatrix4 nMatrix4::operator+(const nMatrix4& otr)const {
		nMatrix4 res;
		for (int i = 0; i < 16; ++i)
		{
			res.A[i] = this->A[i] + otr.A[i];
		}
		return res;
	}
	nMatrix4 nMatrix4::operator-(const nMatrix4& otr)const {
		nMatrix4 res;
		for (int i = 0; i < 16; ++i)
		{
			res.A[i] = this->A[i] - otr.A[i];
		}
		return res;
	}
	nMatrix4 nMatrix4::operator*(const nMatrix4& otr)const {
		nMatrix4 res;
		for (int i = 0; i < 16; ++i)
		{
			res.A[i] = this->A[i] * otr.A[i];
		}
		return res;
	}

	nVector4 nMatrix4::operator*(const nVector4& v_) const {
		return nVector4(
			A[0] * v_.x + A[4] * v_.y + A[8] * v_.z + A[12] * v_.w,
			A[1] * v_.x + A[5] * v_.y + A[9] * v_.z + A[13] * v_.w,
			A[2] * v_.x + A[6] * v_.y + A[10] * v_.z + A[14] * v_.w,
			A[3] * v_.x + A[7] * v_.y + A[11] * v_.z + A[15] * v_.w
		);
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
	float	 Determinant(const nMatrix4& otr)
	{
		const float* m = otr.A;

		return m[0] * (m[5] * (m[10] * m[15] - m[11] * m[14]) -
			   m[9] * (m[6] * m[15] - m[7] * m[14]) +
			   m[13] * (m[6] * m[11] - m[7] * m[10]))

			-  m[1] * (m[4] * (m[10] * m[15] - m[11] * m[14]) -
			   m[8] * (m[6] * m[15] - m[7] * m[14]) +
			   m[12] * (m[6] * m[11] - m[7] * m[10]))

			+  m[2] * (m[4] * (m[9] * m[15] - m[11] * m[13]) -
			   m[8] * (m[5] * m[15] - m[7] * m[13]) +
			   m[12] * (m[5] * m[11] - m[7] * m[9]))

			-  m[3] * (m[4] * (m[9] * m[14] - m[10] * m[13]) -
			   m[8] * (m[5] * m[14] - m[6] * m[13]) +
			   m[12] * (m[5] * m[10] - m[6] * m[9]));
	}

	nMatrix4 RotateX(float rad)
	{
		nVector3 x(1, 0, 0);
		return Rotate(rad, x);
	}
	nMatrix4 RotateY(float rad)
	{
		nVector3 y(0, 1, 0);
		return Rotate(rad, y);
	}
	nMatrix4 RotateZ(float rad)
	{
		nVector3 z(0, 0, 1);
		return Rotate(rad, z);
	}
	nMatrix4 Translate(const nVector3& t)
	{
		nMatrix4 m = Identity();
		m.A[12] = t.x;
		m.A[13] = t.y;
		m.A[14] = t.z;
		return m;
	}
	nMatrix4 Scale(const nVector3& s)
	{
		return nMatrix4{
			s.x, 0, 0, 0,
			0, s.y, 0, 0,
			0, 0, s.z, 0,
			0, 0, 0, 1
		};
	}

	nMatrix4 Transpose(const nMatrix4& otr)
	{	
		nMatrix4 otrT = { 0 };

		for (int c = 0; c < 4; ++c)
		{
			for (int r = 0; r < 4; ++r)
			{
				otrT.A[c * 4 + r] = otr.A[r * 4 + c];
			}
		}

		return otrT;
	}

	nMatrix4 Inverse(const nMatrix4& otr)
	{	//todo .......
		return otr;
	}

	nMatrix4 Rotate(float rad, const nVector3& axis) {
		
		float c = cos(rad);
		float s = sin(rad);
		
		float len = axis.Length();
		float X = axis.x / len;
		float Y = axis.y / len;
		float Z = axis.z / len;


		return nMatrix4{
		c+X*X*(1-c),   X*Y*(1-c)-Z*s,  X*Z*(1-c)+Y*s, 0,
		Y*X*(1-c)+Z*s, c+Y*Y*(1-c),   Y*Z*(1-c)-X*s,  0,
		Z*X*(1-c)-Y*s, Z*Y*(1-c)+X*s,   c+Z*Z*(1-c),  0,
		0,			   0,				0,			  1
		};

	}

}