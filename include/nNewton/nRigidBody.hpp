#pragma once 

#include "nTransform.hpp"

namespace nNewton {
	struct nRigidBodyInfo {
		float MASS_ = 1.0f;
		nVector3 INIT_VELOCITY_;
		nTransform INIT_TRANSFORM_;
		bool IS_STATIC_ = false;
	};
	struct nRigidBody
	{
		float MASS_ = 1.0f;
		nVector3 VELOCITY_;
		nTransform TRANSFORM_;
		bool IS_STATIC_ = false;

		nRigidBody();
		explicit nRigidBody(const nRigidBodyInfo& Info_) : MASS_(Info_.MASS_), VELOCITY_(Info_.INIT_VELOCITY_), TRANSFORM_(Info_.INIT_TRANSFORM_), IS_STATIC_(Info_.IS_STATIC_)
		{ }
	};
}