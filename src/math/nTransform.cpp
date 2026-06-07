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

	nMatrix4  nTransform::ConstrTRS(const nVector3& T , const nQuaternion& R , const nVector3& S){
		
		nMatrix4 model;
		model = to_nMatrix4(R);

		model.A[0] *= S.x;
		model.A[1] *= S.x;
		model.A[2] *= S.x;

		model.A[4] *= S.y;
		model.A[5] *= S.y;
		model.A[6] *= S.y;

		model.A[8] *= S.z;
		model.A[9] *= S.z;
		model.A[10] *= S.z;

		model.A[12] = T.x;
		model.A[13] = T.y;
		model.A[14] = T.z;
		
		return model;
	}

	void nTransform::Inverse()
	{
		nTransform inve;
		inve.m_ROT = QInverse(m_ROT);
		inve.m_POS = Vec_Rotate(inve.m_ROT, -m_POS);
	}

}
