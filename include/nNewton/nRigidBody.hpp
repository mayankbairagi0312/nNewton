#pragma once 

#include "nTransform.hpp"

namespace nNewton {
	struct nRigidBodyInfo {
		float MASS_ = 1.0f;

	};
	struct nRigidBody
	{
		nTransform transform;
		nRigidBody();
		explicit nRigidBody(const nRigidBodyInfo& Info_);	
	};
}