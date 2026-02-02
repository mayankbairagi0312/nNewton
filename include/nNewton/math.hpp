#include <initializer_list>
#include <cmath>

namespace nNewton::nMath
{
//>>>========================== |  MATH UTILITIES | =============================<<<<///

	inline constexpr float PI = 3.1459456f;
	inline constexpr float DegToRad = PI / 180;
	inline constexpr float RadToDeg = 180 / PI;
	inline constexpr float SQRT2 = 1.4142135f;

	constexpr float Radians(float deg)
	{
		return deg * DegToRad;
	}

	constexpr float Degrees(float rad)
	{
		return rad * RadToDeg;
	}


	template<typename T>
	constexpr T Clamp(T v, T min, T max)
	{
		return v < min ? min : (v > max ? max : v);
	}

	template<typename T>
	constexpr T Lerp(const T& a, const T& b, float t)
	{
		return a + (b - a) * t;
	}




//====================== VECTOR 2 =======================//
	struct nVector2
	{
		float x, y;

		constexpr nVector2();
		constexpr explicit nVector2(float m);
		constexpr nVector2(float m_, float c_);
		constexpr nVector2(const nVector2& otr) = default;

		constexpr nVector2 operator+(const nVector2& otr)const;
		constexpr nVector2 operator-(const nVector2& otr)const;
		constexpr nVector2 operator*(const nVector2& otr)const;
		constexpr nVector2 operator-() const;

		constexpr nVector2& operator+=(const nVector2& otr);
		constexpr nVector2& operator-=(const nVector2& otr);
		constexpr nVector2& operator*=(const nVector2& otr);

		constexpr nVector2 operator*(float scalar) const;
		constexpr nVector2 operator/(float scalar) const;
		constexpr nVector2& operator*=(float scalar);
		constexpr nVector2& operator/=(float scalar);


		float Length()const;
	};
	constexpr nVector2 operator*(float scalar, const nVector2& vec);

	constexpr float DotProduct(const nVector2& b, const nVector2& c);
	constexpr float CrossProduct(const nVector2& a, const nVector2& b);
	constexpr nVector2 Perpendicular(const nVector2& v);

	nVector2 Normalized(const nVector2& a);




//====================== VECTOR 3 =======================//
	struct nVector3
	{
		float x, y, z;

		constexpr nVector3();
		constexpr explicit nVector3(float m);
		constexpr nVector3(float m_, float k_, float c_);
		constexpr nVector3(const nVector3& otr) = default;

		constexpr nVector3 operator+(const nVector3& otr)const;
		constexpr nVector3 operator-(const nVector3& otr)const;
		constexpr nVector3 operator*(const nVector3& otr)const;
		constexpr nVector3 operator-() const;

		constexpr nVector3& operator+=(const nVector3& otr);
		constexpr nVector3& operator-=(const nVector3& otr);
		constexpr nVector3& operator*=(const nVector3& otr);

		constexpr nVector3 operator*(float scalar) const;
		constexpr nVector3 operator/(float scalar) const;
		constexpr nVector3& operator*=(float scalar);
		constexpr nVector3& operator/=(float scalar);

		float Length()const;
	};
	constexpr nVector3 operator*(float scalar, const nVector3& vec);
	constexpr float DotProduct(const nVector3& b, const nVector3& c);
	constexpr nVector3 CrossProduct(const nVector3& b, const nVector3& c);
	nVector3 Normalized(const nVector3& a);



//======================VECTOR 4=======================//

	struct nVector4
	{
		float x, y, z, w;

		constexpr nVector4();
		constexpr explicit nVector4(float m);
		constexpr nVector4(float b_, float s_, float d_,float k_);
		constexpr nVector4(const nVector4& otr) = default;

		constexpr nVector4 operator+(const nVector4& otr)const;
		constexpr nVector4 operator-(const nVector4& otr)const;
		constexpr nVector4 operator*(const nVector4& otr)const;
		constexpr nVector4 operator-() const;

		constexpr nVector4& operator+=(const nVector4& otr);
		constexpr nVector4& operator-=(const nVector4& otr);
		constexpr nVector4& operator*=(const nVector4& otr);

		constexpr nVector4 operator*(float scalar) const;
		constexpr nVector4 operator/(float scalar) const;
		constexpr nVector4& operator*=(float scalar);
		constexpr nVector4& operator/=(float scalar);

		float Length()const;
	};
	constexpr	nVector4	operator*	(float scalar, const nVector4& vec);
	constexpr	float		DotProduct	(const nVector4& b, const nVector4& c);
	constexpr	nVector4	CrossProduct(const nVector4& b, const nVector4& c);
				nVector4	Normalized	(const nVector4& a);


//++============================ MATRIX 4X4 ===================================++//
// 
// Column Major: A[col * 4 + row]

	struct nMatrix4
	{
		float A[16];

		constexpr nMatrix4();
		constexpr explicit nMatrix4(float m);
		constexpr nMatrix4(std::initializer_list<float> values);
		constexpr nMatrix4(const nMatrix4& otr) = default;

					nMatrix4 operator+(const nMatrix4& otr)const;
					nMatrix4 operator-(const nMatrix4& otr)const;
					nMatrix4 operator*(const nMatrix4& otr)const;

					nVector4 operator*(const nVector4& v) const;

	};

	
	  constexpr	nMatrix4 Identity	();
				float	 Determinant(const nMatrix4& otr);

				nMatrix4 RotationX	(float rad);
				nMatrix4 RotationY	(float rad);
				nMatrix4 RotationZ	(float rad);
				nMatrix4 Translation(const nVector3& t);
				nMatrix4 Scale		(const nVector3& s);

				nMatrix4 Transpose	(const nMatrix4& otr);
				nMatrix4 Inverse	(const nMatrix4& otr);

				nMatrix4 Rotation	(float rad, const nVector3& axis);

	

//++============================ QUATERNION ===================================++//

				/// TODO :) 
}