#include <nNewton/nMath.hpp>
#include <cassert>
#include <cmath>

namespace nNewton
{
//VECTOR 2

	float nVector2::Length()const
	{
		return sqrt(x * x + y * y);
	}

	nVector2 Normalized(const nVector2& a) {
		float len = a.Length();
		if (len == 0.0f)
			return nVector2(0.0f);
		return a / len;
	}


	//==========  VECTOR 3 

	

	float nVector3::Length()const
	{
		return sqrt(x * x + y * y + z*z);
	}

	nVector3 Normalized(const nVector3& a) {
		float len = a.Length();
		if (len == 0.0f)
			return nVector3(0.0f);
		return a / len;
	}

	//======================VECTOR 4=======================//

	float nVector4::Length()const
	{
		return sqrt(x * x + y * y + z * z + w * w);
	}

	nVector4 Normalized(const nVector4& a) {
		float len = a.Length();
		if (len == 0.0f)
			return nVector4(0.0f);
		return a / len;
	}

	//=====    MATRIX 4 


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
			c + X * X * (1 - c),
			Y* X* (1 - c) + Z * s,
			Z* X* (1 - c) - Y * s,
			0,

			X* Y* (1 - c) - Z * s,
			c + Y * Y * (1 - c),
			Z* Y* (1 - c) + X * s,
			0,

			X* Z* (1 - c) + Y * s,
			Y* Z* (1 - c) - X * s,
			c + Z * Z * (1 - c),
			0,

			0, 0, 0, 1
		};

	}

	nMatrix4 Look_At(const nVector3& eye_, const nVector3& center_, const nVector3& up_)
	{
		
		auto fwd_	= Normalized(center_ - eye_ );
		auto side_ = Normalized(CrossProduct(fwd_, up_));
		auto Up_ = Normalized(CrossProduct(side_, fwd_));

		return nMatrix4{
			side_.x,Up_.x,-fwd_.x,0,
			side_.y,Up_.y,-fwd_.y,0,
			side_.z,Up_.z,-fwd_.z,0,
			-DotProduct(side_,eye_),-DotProduct(Up_,eye_),DotProduct(fwd_,eye_),1

		};
	}


	nMatrix4 Perspective(float FOV_, float aspect_, float nearZ_, float farZ_)
	{
		float f = 1.0f / tan(FOV_ / 2.0f);

		return {
		f / aspect_, 0, 0, 0,
		0, f, 0, 0,
		0, 0, -(farZ_ + nearZ_) / (farZ_ - nearZ_), -1,
		0, 0, -(2.0f * farZ_ * nearZ_) / (farZ_ - nearZ_), 0
		};
	}

	nMatrix4 Ortho(float Left_, float Right_, float Bottom_, float Top_, float nearZ_, float farZ_)
	{
		return {
		2/(Right_- Left_),0,0,0,

		0,2/(Top_-Bottom_),0,0,

		0,0,-2/(farZ_-nearZ_),0,

		-(Right_+Left_)/ (Right_ - Left_),-(Top_ + Bottom_)/ (Top_ - Bottom_),-(farZ_ + nearZ_)/ (farZ_ - nearZ_),1
		};
	}

// Quaternions ===========>>>>>>>>>>>

	nQuaternion operator* (float scalar_, const nQuaternion& Quat_)
	{
		return { scalar_ * Quat_.w, scalar_ * Quat_.x, scalar_ * Quat_.y, scalar_ * Quat_.z };

	}

	float nQuaternion::Length()const
	{
		return sqrt(w * w + x * x + y * y + z * z);
	}



	nQuaternion QNormalize(const nQuaternion& Quat_)
	{
		float len = Quat_.Length();
		if (len == 0.0f)
			return nQuaternion(1, 0, 0, 0);
		float mag = Quat_.Length();
		return { Quat_.w / mag ,Quat_.x / mag, Quat_.y / mag, Quat_.z / mag };
	}
	nQuaternion Conjugate(const nQuaternion& Quat_)
	{
		return { Quat_.w,-Quat_.x,-Quat_.y ,-Quat_.z };
	}
	nQuaternion QInverse(const nQuaternion& Quat_)
	{
		
		float lensq = Quat_.w * Quat_.w + Quat_.x * Quat_.x + Quat_.y * Quat_.y + Quat_.z * Quat_.z;

		if (lensq == 0.0f)
			return nQuaternion(1, 0, 0, 0);

		nQuaternion conj_ = Conjugate(Quat_);
		
		if (fabsf(lensq - 1.0f) < 1e-6f)
			return conj_;
		
		return { conj_.w / lensq, conj_.x / lensq, conj_.y / lensq, conj_.z / lensq };
	}

	float QDotProduct(const nQuaternion& Quat1_, const nQuaternion& Quat2_)
	{
		return Quat1_.w * Quat2_.w +
			Quat1_.x * Quat2_.x +
			Quat1_.y * Quat2_.y +
			Quat1_.z * Quat2_.z;
	}

	nQuaternion from_AxisAngle(const nVector3& Axis_, float AngleRad_)
	{	
		nVector3 axis = Axis_;

		float lensq = Axis_.x * Axis_.x + Axis_.y * Axis_.y + Axis_.z * Axis_.z;
		if (lensq == 0.0f)
			return nQuaternion(1, 0, 0, 0);

		if (fabsf(lensq - 1.0f) > 1e-6f)
			axis = Normalized(Axis_);


		auto half_angle = AngleRad_ / 2;
		auto s = sin(half_angle);

		return { cos(half_angle), axis.x * s,
			axis.y * s, axis.z * s };

	}

	nQuaternion from_AngularVelocity(const nVector3& omega_, float dt_)
	{
		float lensq = omega_.x * omega_.x + omega_.y * omega_.y + omega_.z * omega_.z;
		if (lensq == 0.0f)
			return nQuaternion(1, 0, 0, 0);

		auto len = omega_.Length();
		

		auto omega = omega_ /len;
		auto angle = len * dt_;
		auto half_angle_ = angle / 2;
		auto s = sin(half_angle_);

		return { cos(half_angle_), omega.x * s,
			omega.y * s, omega.z * s };
	}

	nMatrix4 to_nMatrix4(const nQuaternion& Quat_)
	{
		float w = Quat_.w;
		float x = Quat_.x;
		float y = Quat_.y;
		float z = Quat_.z;

		float xx = x * x;
		float yy = y * y;
		float zz = z * z;
		float xy = x * y;
		float xz = x * z;
		float yz = y * z;
		float wx = w * x;
		float wy = w * y;
		float wz = w * z;

		return nMatrix4{
			1-2*(yy + zz), 2 *(xy + wz), 2 * (xz - wy), 0,
			2*(xy - wz), 1 - 2 * (xx + zz), 2 * (yz + wx), 0,
			2*(xz + wy), 2 * (yz - wx),1 - 2 * (xx + yy), 0,
			0, 0, 0,1
		};
	}

	nVector3 Vec_Rotate(const nQuaternion& Quat_, const nVector3& Vec3_)
	{	
		auto inv_ = QInverse(Quat_);
		nVector3 v(Quat_.x, Quat_.y, Quat_.z);

		nQuaternion ro(0, Vec3_.x, Vec3_.y, Vec3_.z);
		nQuaternion q( Quat_ * ro * inv_ );

		return nVector3(q.x, q.y, q.z);
	}

	nQuaternion QIntegrate(nQuaternion orientation_, const nVector3& angularV_, float dt_)
	{	
		float Lensq = DotProduct(angularV_, angularV_);
		if (Lensq < 1e-8f)
			return orientation_;

		//float Len = sqrt(Lensq);
		///auto axis = angularV_/ Len;
		//auto dq = from_AxisAngle( axis,Len*dt_);

		auto dq = 0.5f * orientation_ * nQuaternion(0, angularV_.x, angularV_.y, angularV_.z)*dt_;

		orientation_ = orientation_ +  dq;

		return QNormalize(orientation_);
	}

	nQuaternion QSlerp(const nQuaternion& Quat1_, const nQuaternion& Quat2_, float t_)
	{
		auto Q1 = QNormalize(Quat1_);
		auto Q2 = QNormalize(Quat2_);

		auto dot = QDotProduct(Quat1_, Quat2_);

		if (dot < 0.0f)
		{
			Q2 = { -Q2.w , -Q2.x , -Q2.y , -Q2.z} ;

			dot = -dot;
		}

		auto angle = acos(Clamp(dot, -1.0f, 1.0f));

		if (dot > 1.0f - EPSILON) {
			return QNormalize((1.0f - t_) * Q1 + t_ * Q2);
		}

		float s = sin(angle);

		return (sin((1-t_)*angle)/s)*Quat1_  + (sin((t_) * angle) / s) * Quat2_;
	}

	nQuaternion QNlerp(const nQuaternion& Quat1_, const nQuaternion& Quat2_, float t_)
	{
		auto Q1 = QNormalize(Quat1_);
		auto Q2 = QNormalize(Quat2_);

		if (QDotProduct(Quat1_, Quat2_) < 0.0f)
		{
			Q2 = { -Q2.w , -Q2.x , -Q2.y , -Q2.z };

		}

		return QNormalize((1.0f - t_) * Q1 + t_ * Q2);
	}
}