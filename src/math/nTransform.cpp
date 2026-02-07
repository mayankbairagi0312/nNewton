#include<nNewton/nTransform.hpp>

namespace nNewton {


	nTransform::nTransform(): m_POS(0),m_ROT(1,0,0,0),m_SCALE(1)
	{}

	nTransform::nTransform(nVector3 pos,nQuaternion rot, nVector3 Scale): m_POS(pos),m_ROT(rot),m_SCALE(Scale)
	{}

	void nTransform::Rotate(const nVector3& axis_, float Rad_)
	{
		auto Quat = from_AxisAngle(axis_, Rad_);
		m_ROT = m_ROT * Quat;
	}

	void nTransform::Rotate(const nQuaternion& Quat_)
	{
		m_ROT = QNormalize(Quat_ * m_ROT);
	}


	nVector3 nTransform::TransformPt(const nVector3& LPoint_)const
	{
		return m_POS + Vec_Rotate(m_ROT, LPoint_);
	}

	nVector3 nTransform::TransformVec(const nVector3& LVector_)const
	{
		return Vec_Rotate(m_ROT, LVector_);
	}

	nVector3 nTransform::InvTransfromPt(const nVector3& WPoint_)const
	{
		return Vec_Rotate(QInverse(m_ROT), WPoint_-m_POS);
	}

	nVector3 nTransform::InvTransfromVec(const nVector3& WVector_)const
	{
		return Vec_Rotate(QInverse(m_ROT), WVector_);
	}

	void nTransform::Inverse()
	{
		nTransform inve;
		inve.m_ROT = QInverse(m_ROT);
		inve.m_POS = Vec_Rotate(inve.m_ROT, -m_POS);
	}

}
