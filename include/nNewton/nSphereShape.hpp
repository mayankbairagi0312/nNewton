#pragma once

#include "nMath.hpp"
#include "nCollisionShapes.hpp"
#include "nTransform.hpp"
#include "nAABB.hpp"

namespace nNewton
{
	class nSphereShape 
	{
	public:
		float radius;
		nSphereShape() = default;
		explicit nSphereShape(float radi) : radius(radi) {}
		
		static const nCollisionShapeType nTYPE = nCollisionShapeType::nSphere;

		nAABB getAABB(const nTransform& transform_)const
		{
			auto WCenter = transform_.GetPosition();
			float WRadius = radius * transform_.GetScale().x;
			nVector3 ext(WRadius, WRadius, WRadius);

			return nAABB(WCenter - ext , WCenter + ext);

		}

		// farthest point on the shape in a given direction (local space).
		nVector3 getSupportPoint(const nVector3& direction)const 
		{
			auto len = direction.Length();
			if (len  < EPSILON) return nVector3(0, 0, 0);
			
			auto normDir = direction / len;
			return normDir * radius;
		}

		nVector3 getCentroid() const { return nVector3{ 0.0f, 0.0f, 0.0f };}

		float getVolume() const { return (4.0f / 3.0f) * PI * radius * radius * radius;}

		// Unit inertia tensor (I/mass) – all diagonal elements equal: (2/5)·r²
		nMatrix3 getUnitInertia() const 
		{ 
			const float I = (2.0f / 5.0f) * radius * radius;
			return nMatrix3{
				I, 0.0f, 0.0f,   
				0.0f, I, 0.0f,   
				0.0f, 0.0f, I    
			};
		}
	};
}