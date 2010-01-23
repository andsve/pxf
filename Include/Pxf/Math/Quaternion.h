#ifndef __PXF_MATH_QUATERNION_H__
#define __PXF_MATH_QUATERNION_H__

#include <Pxf/Math/Math.h>
#include <Pxf/Math/Matrix.h>
#include <Pxf/Math/Vector.h>
//#include <Pxf/Util/Debug.h>

namespace Pxf {
namespace Math {

class Quaternion
{
public:
	float x;
	float y;
	float z;
	float w;

	Quaternion()
		: x(0.f), y(0.f), z(0.f) ,w(1.f) //initial state: no rotation
	{}

	Quaternion(const Vec3f& in_vec, const float& in_deg)
	{
		CreateFromAxisAngleDeg(in_vec, in_deg);
	}

	Quaternion(const float& in_roll, const float& in_yaw, const float& in_pitch)
	{
		CreateFromEulerAngles(in_roll, in_yaw, in_pitch);
	}

	void Set(float in_x, float in_y, float in_z, float in_w)
	{
		x = in_x;
		y = in_y;
		z = in_z;
		w = in_w;
	}

	void SetIdentity()
	{
		Set(0, 0, 0, 1);
	}

	// create from axis angle
	void CreateFromAxisAngle(const Vec3f& in_vec, const float& in_rad)
	{
		float res = sin(in_rad/2.f);
		x = in_vec.x * res;
		y = in_vec.y * res;
		z = in_vec.z * res;
		w = cos(in_rad/2.f);
	}

	void CreateFromAxisAngleDeg(const Vec3f& in_vec, const float& in_deg)
	{
		CreateFromAxisAngle(in_vec, (in_deg/180.f) * PI);
	}

	void CreateFromEulerAngles(const float& _yaw, const float& _pitch, const float& _roll)
	{
		float cr, cp, cy, sr, sp, sy, cpcy, spsy;
		cr = cos(_yaw/2.f);
		cp = cos(_pitch/2.f);
		cy = cos(_roll/2.f);

		sr = sin(_yaw/2.f);
		sp = sin(_pitch/2.f);
		sy = sin(_roll/2.f);

		cpcy = cp * cy;
		spsy = sp * sy;

		w = cr * cpcy + sr * spsy;
		x = sr * cpcy - cr * spsy;
		y = cr * sp * cy + sr * cp * sy;
		z = cr * cp * sy - sr * sp * cy;
	}

	void CreateFromEulerAnglesDeg(const float& _pitch, const float& _yaw, const float& _roll)
	{
		CreateFromEulerAngles(Math::Deg2Rad(_pitch), Math::Deg2Rad(_yaw), Math::Deg2Rad(_roll));
	}

	void CreateFromMatrix(const Mat4* in_mtx)
	{
		const float *m = (float*)(&in_mtx->m);
		float s = 0.0f;
		float q[4] = {0.0f};
		float trace = m[0] + m[5] + m[10];

		if (trace > 0.0f)
		{
			s = sqrtf(trace + 1.0f);
			q[3] = s * 0.5f;
			s = 0.5f / s;
			q[0] = (m[6] - m[9]) * s;
			q[1] = (m[8] - m[2]) * s;
			q[2] = (m[1] - m[4]) * s;
		}
		else
		{
			int nxt[3] = {1, 2, 0};
			int i = 0, j = 0, k = 0;

			if (m[5] > m[0])
				i = 1;

			if (m[10] > m[(i*4)+i])
				i = 2;

			j = nxt[i];
			k = nxt[j];
			s = sqrtf((m[i*4+i] - (m[j*4+j] + m[k*4+k])) + 1.0f);

			q[i] = s * 0.5f;
			s = 0.5f / s;
			q[3] = (m[j*4+k] - m[k*4+j]) * s;
			q[j] = (m[i*4+j] + m[j*4+i]) * s;
			q[k] = (m[i*4+k] + m[k*4+i]) * s;
		}

		x = q[0], y = q[1], z = q[2], w = q[3];
	}

	void CreateMatrix(Mat4* out_mtx) const
	{
		out_mtx->m[ 0] = 1.0f - 2.0f * ( y * y + z * z );  
		out_mtx->m[ 1] = 2.0f * ( x * y - w * z );  
		out_mtx->m[ 2] = 2.0f * ( x * z + w * y );  
		out_mtx->m[ 3] = 0.0f;  

		out_mtx->m[ 4] = 2.0f * ( x * y + w * z );  
		out_mtx->m[ 5] = 1.0f - 2.0f * ( x * x + z * z );  
		out_mtx->m[ 6] = 2.0f * ( y * z - w * x );  
		out_mtx->m[ 7] = 0.0f;  

		out_mtx->m[ 8] = 2.0f * ( x * z - w * y );  
		out_mtx->m[ 9] = 2.0f * ( y * z + w * x );  
		out_mtx->m[10] = 1.0f - 2.0f * ( x * x + y * y );  
		out_mtx->m[11] = 0.0f;  

		out_mtx->m[12] = 0;  
		out_mtx->m[13] = 0;  
		out_mtx->m[14] = 0;  
		out_mtx->m[15] = 1.0f;
	}

	/*
		Operators
	*/

	/* Assign */
	const Quaternion& operator = (const Quaternion& o)
	{
		x = o.x;
		y = o.y;
		z = o.z;
		w = o.w;
		return *this;
	}

	/* Negation */
	Quaternion operator - () const
	{	
		Quaternion q;
		q.x = -x;
		q.y = -y;
		q.z = -z;
		q.w = -w;
		return q;
	}

	/* Subtraction */
	Quaternion operator - (const Quaternion& o) const
	{
		Quaternion q;
		q.x = x-o.x;
		q.y = y-o.y;
		q.z = z-o.z;
		q.w = w-o.w;
		return q;
	}

	/* Addition */
	Quaternion operator + (const Quaternion& o) const
	{
		Quaternion q;
		q.x = x+o.x;
		q.y = y+o.y;
		q.z = z+o.z;
		q.w = w+o.w;
		return q;
	}

	/* Multiplication */
	Quaternion operator * (const float c) const
	{ 
		Quaternion q;
		q.x = x*c;
		q.y = y*c;
		q.z = z*c;
		q.w = w*c;
		return q;
	}
	
	const Quaternion& operator *= (const float c)
	{
		x*=c; y*=c; z*=c; w*=c;
	}
	
	Quaternion operator * (const Quaternion& o) const
	{
		Quaternion tmp;
		tmp.x = w * o.x + x * o.w + y * o.z - z * o.y;
		tmp.y = w * o.y + y * o.w + z * o.x - x * o.z;
		tmp.z = w * o.z + z * o.w + x * o.y - y * o.x;
		tmp.w = w * o.w - x * o.x - y * o.y - z * o.z;
		return tmp;
	}

	const Quaternion& operator *= (const Quaternion& o)
	{
		Quaternion tmp;
		tmp.x = w * o.x + x * o.w + y * o.z - z * o.y;
		tmp.y = w * o.y + y * o.w + z * o.x - x * o.z;
		tmp.z = w * o.z + z * o.w + x * o.y - y * o.x;
		tmp.w = w * o.w - x * o.x - y * o.y - z * o.z;
		*this = tmp;
		return *this;
	}
	
	/* Division */
	Quaternion operator / (const float c) const
	{
		Quaternion q;
		q.x = x/c;
		q.y = y/c;
		q.z = z/c;
		q.w = w/c;
		return q;
	}

};

/* Length */
inline float LengthSqr(const Quaternion& q)
{
	return q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w;
}

inline float Length(const Quaternion& q)
{
	return sqrtf(LengthSqr(q));
}

inline void Normalize(Quaternion& q)
{
	float Len = Length(q);
	if (Len > 0.0000001f)
	{
		q.x /= Len;
		q.y /= Len;
		q.z /= Len;
		q.w /= Len;
	}
	else
		q.x = q.y = q.z = q.w = 0.f;
}

inline Quaternion Normalized(Quaternion& q)
{
	Quaternion nq = q;
	Normalize(nq);
	return nq;
}

inline void Conjugate(Quaternion& q)
{
	q.x = -q.x;
	q.y = -q.y;
	q.z = -q.z;
}

inline Quaternion Conjugated(const Quaternion& q)
{
	Quaternion nq = q;
	Conjugate(nq);
	return nq;
}

inline float Dot(const Quaternion& a, const Quaternion& b)
{ 
	return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

inline Quaternion Lerp(const Quaternion& from, const Quaternion& to, float t)
{
	return from + (to - from)*t;
}

inline Quaternion Slerp(const Quaternion& from, const Quaternion& to, float t)
{
	if (t <= 0.0f)
		return from;
	if (t >= 1.0f)
		return to;

	Quaternion q3 = to;
	float c = Dot(from, q3);

	if (c < 0.0f)
	{
		q3 = -q3;
		c = -c;
	}

	if (c > 1.0f - 0.0000001f)
	{	
		Quaternion q = Lerp(from, q3, t);
		return Normalized(q);
	}

	float a = Clamp(acosf(c), -1.f, 1.f);
	return (from*sinf((1.0f - t) * a) + q3*sinf(t * a)) / sinf(a);
}


// Get direction of quaternion
inline Vec3f Direction(const Quaternion& q)
{
	Vec3f d(2.0f * (q.x * q.z - q.w * q.y)
		   ,2.0f * (q.y * q.z + q.w * q.x)
		   ,1.0f - 2.0f * (q.x * q.x + q.y * q.y));
	Normalize(d);
	return d;
}

} // Math
} // Pxf

#endif // __PXF_MATH_QUATERNION_H__
