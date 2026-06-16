#pragma once
#include "nMath.hpp"

namespace nNewton {
	class nTransform
	{
	private:
		nVector3 m_POS;
		nQuaternion m_ROT;
		nVector3 m_SCALE;

	public:

		//CTOR

		nTransform();
		nTransform(nVector3 pos, nQuaternion rot, nVector3 Scale);
		//seters
		void SetPosition(const nVector3& pos_){m_POS = pos_;}
		void SetRotation(const nQuaternion& rot_) { m_ROT = rot_; }
		void SetScale(const nVector3& scale_) { m_SCALE = scale_; }

		//geters
		nVector3 GetPosition()const { return m_POS; }
		nQuaternion GetRotation()const { return m_ROT; }
		nVector3 GetScale()const { return m_SCALE; }


		nVector3 Right()const { return Vec_Rotate(m_ROT, { 1.0f, 0.0f, 0.0f }); }
		nVector3 Up()const { return Vec_Rotate(m_ROT, { 0.0f, 1.0f, 0.0f }); }
		nVector3 Forward()const { return Vec_Rotate(m_ROT, { 0.0f, 0.0f, 1.0f }); }

		void Rotate(const nVector3& axis_,float Rad_);
		void Rotate(const nQuaternion& Quat_);

		
		nVector3 TransformPt(const nVector3& LPoint_)const;
		nVector3 TransformVec(const nVector3& LVector_)const;
		nVector3 InvTransfromPt(const nVector3& WPoint_)const;
		nVector3 InvTransfromVec(const nVector3& WVector_)const;

		static nMatrix4  ConstrTRS(const nVector3& T, const nQuaternion& R, const nVector3& S);
		nMatrix4 ToMatrix()const;

		nTransform operator*(const nTransform& rhs)const;
		static nTransform ComposeTransform(const nTransform& parent, const nTransform& child);
		nTransform Inverse() const;
		void Invert();

		static nTransform Lerp(const nTransform& a, const nTransform& b, float t);

	};
}
