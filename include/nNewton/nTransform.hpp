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

		void Rotate(const nVector3& axis_,float Rad_);
		void Rotate(const nQuaternion& Quat_);

		
		nVector3 TransformPt(const nVector3& LPoint_)const;
		nVector3 TransformVec(const nVector3& LVector_)const;
		nVector3 InvTransfromPt(const nVector3& WPoint_)const;
		nVector3 InvTransfromVec(const nVector3& WVector_)const;

		void Inverse();
	};
}
