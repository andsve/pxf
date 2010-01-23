#ifndef __PXF_MATH_MATRIX_H__
#define __PXF_MATH_MATRIX_H__

#include <Pxf/Math/Math.h>
#include <Pxf/Math/Vector.h>
#include <Pxf/Base/Debug.h>

namespace Pxf {
namespace Math {

// implement more operators...

class Mat4
{
public:

	static const Mat4 Zero;
	static const Mat4 Identity;

	float m[16];

	Mat4()
	{
		*this = Zero;
	}

	Mat4(float m00, float m01, float m02, float m03,
		 float m10, float m11, float m12, float m13,
		 float m20, float m21, float m22, float m23,
		 float m30, float m31, float m32, float m33)
	{
		m[ 0] = m00; m[ 1] = m01; m[ 2] = m02; m[ 3] = m03;
		m[ 4] = m10; m[ 5] = m11; m[ 6] = m12; m[ 7] = m13;
		m[ 8] = m20; m[ 9] = m21; m[10] = m22; m[11] = m23;
		m[12] = m30; m[13] = m31; m[14] = m32; m[15] = m33;
	}

	Mat4(const Mat4& o)
	{
		*this = o;
	}

	Mat4& operator = (const Mat4& o)
	{
		m[ 0] = o.m[ 0]; m[ 1] = o.m[ 1]; m[ 2] = o.m[ 2]; m[ 3] = o.m[ 3];
		m[ 4] = o.m[ 4]; m[ 5] = o.m[ 5]; m[ 6] = o.m[ 6]; m[ 7] = o.m[ 7];
		m[ 8] = o.m[ 8]; m[ 9] = o.m[ 9]; m[10] = o.m[10]; m[11] = o.m[11];
		m[12] = o.m[12]; m[13] = o.m[13]; m[14] = o.m[14]; m[15] = o.m[15];
		return *this;
	}

	// Rewrite it to be efficient
	const Mat4& operator *= (const Mat4& o)
	{
		*this = *this * o;
		return *this;
	}

	Mat4 operator * (const Mat4& o) const
	{
		Mat4 res;

		// unwind this?
		for (int y = 0; y < 4; ++y)
		{
			for (int x = 0; x < 4; ++x)
			{
				res.m[x*4+y] = m[y     ] * o.m[x*4    ] +
					m[y +  4] * o.m[x*4 + 1] +
					m[y +  8] * o.m[x*4 + 2] +
					m[y + 12] * o.m[x*4 + 3];
			}
		}

		return res;
	}

	Mat4 operator + (const Mat4& o) const
	{
		Mat4 res;
		for (int i = 0; i < 16; i++)
			res.m[i] = m[i] + o.m[i];
		return res;
	}

	Mat4 operator - (const Mat4& o) const
	{
		Mat4 res;
		for (int i = 0; i < 16; i++)
			res.m[i] = m[i] - o.m[i];
		return res;
	}

	float Det() const
	{
		
		return	m[ 0]*m[ 5]*m[10]*m[15] + m[ 0]*m[ 9]*m[14]*m[ 7] + m[ 0]*m[13]*m[ 6]*m[11] +
				m[ 4]*m[ 1]*m[14]*m[11] + m[ 4]*m[ 9]*m[ 2]*m[15] + m[ 4]*m[13]*m[10]*m[ 3] +
				m[ 8]*m[ 1]*m[ 6]*m[15] + m[ 8]*m[ 5]*m[14]*m[ 3] + m[ 8]*m[13]*m[ 2]*m[ 7] +
				m[12]*m[ 1]*m[10]*m[ 7] + m[12]*m[ 5]*m[ 2]*m[11] + m[12]*m[ 9]*m[ 6]*m[ 3] -
				m[ 0]*m[ 5]*m[14]*m[11] - m[ 0]*m[ 9]*m[ 6]*m[15] - m[ 0]*m[13]*m[10]*m[ 7] -
				m[ 4]*m[ 1]*m[10]*m[15] - m[ 4]*m[ 9]*m[14]*m[ 3] - m[ 4]*m[13]*m[ 2]*m[11] -
				m[ 8]*m[ 1]*m[14]*m[ 7] - m[ 8]*m[ 5]*m[ 2]*m[15] - m[ 8]*m[13]*m[ 6]*m[ 3] -
				m[12]*m[ 1]*m[ 6]*m[11] - m[12]*m[ 5]*m[10]*m[ 3] - m[12]*m[ 9]*m[ 2]*m[ 7];
	}


	const Mat4& Inv()
	{
		
		Mat4 mat = Zero;
		float determ = Det();
		mat.m[0] = mat.m[5] = mat.m[10] = mat.m[15] = 0;

		if (determ == 0)
		{
			Message("Matrix4", "Inverse with determinant == 0");
			return Zero;
		}

		mat.m[ 0] = m[ 5]*m[10]*m[15] + m[ 9]*m[14]*m[ 7] + m[13]*m[ 6]*m[11] - m[ 5]*m[14]*m[11] - m[ 9]*m[ 6]*m[15] - m[13]*m[10]*m[ 7];
		mat.m[ 4] = m[ 4]*m[14]*m[11] + m[ 8]*m[ 6]*m[15] + m[12]*m[10]*m[ 7] - m[ 4]*m[10]*m[15] - m[ 8]*m[14]*m[ 7] - m[12]*m[ 6]*m[11];
		mat.m[ 8] = m[ 4]*m[ 9]*m[15] + m[ 8]*m[13]*m[ 7] + m[12]*m[ 5]*m[11] - m[ 4]*m[13]*m[11] - m[ 8]*m[ 5]*m[15] - m[12]*m[ 9]*m[ 7];
		mat.m[12] = m[ 4]*m[13]*m[10] + m[ 8]*m[ 5]*m[14] + m[12]*m[ 9]*m[ 6] - m[ 4]*m[ 9]*m[14] - m[ 8]*m[13]*m[ 6] - m[12]*m[ 5]*m[10];
		mat.m[ 1] = m[ 1]*m[14]*m[11] + m[ 9]*m[ 2]*m[15] + m[13]*m[10]*m[ 3] - m[ 1]*m[10]*m[15] - m[ 9]*m[14]*m[ 3] - m[13]*m[ 2]*m[11];
		mat.m[ 5] = m[ 0]*m[10]*m[15] + m[ 8]*m[14]*m[ 3] + m[12]*m[ 2]*m[11] - m[ 0]*m[14]*m[11] - m[ 8]*m[ 2]*m[15] - m[12]*m[10]*m[ 3];
		mat.m[ 9] = m[ 0]*m[13]*m[11] + m[ 8]*m[ 1]*m[15] + m[12]*m[ 9]*m[ 3] - m[ 0]*m[ 9]*m[15] - m[ 8]*m[13]*m[ 3] - m[12]*m[ 1]*m[11];
		mat.m[13] = m[ 0]*m[ 9]*m[14] + m[ 8]*m[13]*m[ 2] + m[12]*m[ 1]*m[10] - m[ 0]*m[13]*m[10] - m[ 8]*m[ 1]*m[14] - m[12]*m[ 9]*m[ 2];
		mat.m[ 2] = m[ 1]*m[ 6]*m[15] + m[ 5]*m[14]*m[ 3] + m[13]*m[ 2]*m[ 7] - m[ 1]*m[14]*m[ 7] - m[ 5]*m[ 2]*m[15] - m[13]*m[ 6]*m[ 3];
		mat.m[ 6] = m[ 0]*m[14]*m[ 7] + m[ 4]*m[ 2]*m[15] + m[12]*m[ 6]*m[ 3] - m[ 0]*m[ 6]*m[15] - m[ 4]*m[14]*m[ 3] - m[12]*m[ 2]*m[ 7];
		mat.m[10] = m[ 0]*m[ 5]*m[15] + m[ 4]*m[13]*m[ 3] + m[12]*m[ 1]*m[ 7] - m[ 0]*m[13]*m[ 7] - m[ 4]*m[ 1]*m[15] - m[12]*m[ 5]*m[ 3];
		mat.m[14] = m[ 0]*m[13]*m[ 6] + m[ 4]*m[ 1]*m[14] + m[12]*m[ 5]*m[ 2] - m[ 0]*m[ 5]*m[14] - m[ 4]*m[13]*m[ 2] - m[12]*m[ 1]*m[ 6];
		mat.m[ 3] = m[ 1]*m[10]*m[ 7] + m[ 5]*m[ 2]*m[11] + m[ 9]*m[ 6]*m[ 3] - m[ 1]*m[ 6]*m[11] - m[ 5]*m[10]*m[ 3] - m[ 9]*m[ 2]*m[ 7];
		mat.m[ 7] = m[ 0]*m[ 6]*m[11] + m[ 4]*m[10]*m[ 3] + m[ 8]*m[ 2]*m[ 7] - m[ 0]*m[10]*m[ 7] - m[ 4]*m[ 2]*m[11] - m[ 8]*m[ 6]*m[ 3];
		mat.m[11] = m[ 0]*m[ 9]*m[ 7] + m[ 4]*m[ 1]*m[11] + m[ 8]*m[ 5]*m[ 3] - m[ 0]*m[ 5]*m[11] - m[ 4]*m[ 9]*m[ 3] - m[ 8]*m[ 1]*m[ 7];
		mat.m[15] = m[ 0]*m[ 5]*m[10] + m[ 4]*m[ 9]*m[ 2] + m[ 8]*m[ 1]*m[ 6] - m[ 0]*m[ 9]*m[ 6] - m[ 4]*m[ 1]*m[10] - m[ 8]*m[ 5]*m[ 2];

		determ = 1.0f/determ;

		for (int num = 0; num < 16; ++num)
			mat.m[num] *= determ;

		*this = mat;

		return *this;
	}

	const Mat4& Translate(const Vec3f& pos)
	{
		return Translate(pos.x, pos.y, pos.z);
	}
	
	const Mat4& Translate(float x, float y, float z)
	{
		*this = Identity;
		m[12] = x;
		m[13] = y;
		m[14] = z;

		return *this;
	}
	
	static Mat4 Perspective(float fovy, float aspect, float znear, float zfar)
	{
		float f = 1.0f/tanf(fovy*(PI/360.0f));
		Mat4 res = Identity;
		
		res.m[0] = f/aspect;
		res.m[5] = f;
		res.m[10] = (zfar+znear)/(znear-zfar);
		res.m[11] = -1.0f;
		res.m[14] = (2.0f*zfar*znear)/(znear-zfar);
		res.m[15] = 0.0f;

		return res;
	}

	// far (!), near -> compiler errors, renamed to _near, _far
	static Mat4 Ortho(float left, float right, float bottom, float top, float _near, float _far)
	{
		Mat4 res = Identity;

		res.m[0] = 2.0f/(right-left);
		res.m[5] = 2.0f/(top-bottom);
		res.m[10] = -2.0f/(_far-_near);
		res.m[12] = -((right+left)/(right-left));
		res.m[13] = -((top+bottom)/(top-bottom));
		res.m[14] = -((_far+_near)/(_far-_near));

		return res;
	}

	static Mat4 LookAt(const Vec3f& eye_pos, const Vec3f& lookat_pos, const Vec3f& up_vec)
	{
		Vec3f f = Normalized(lookat_pos - eye_pos);
		Vec3f up = Normalized(up_vec);
		Vec3f s = Cross(f, up);
		Vec3f u = Cross(s, f);

		Mat4 R = Mat4(
				s.x, u.x, -f.x, 0.0f,
				s.y, u.y, -f.y, 0.0f,
				s.z, u.z, -f.z, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);

		Mat4 T;
		T.Translate(-eye_pos);

		return R * T;
	}

	// TODO: Hmm... I don't think this works...
	// http://en.wikipedia.org/wiki/Rotation_matrix#Axis_and_angle
	
	static Mat4 Rotate(float angle, float x, float y, float z)
	{
		float c, C, s;
		Vec4f v(x, y, z, 1.f);
		Normalize(v);

		//x = v.x;
		//y = v.y;
		//z = v.z;

		c = cos(angle);
		s = sin(angle);
		C = 1.f - c;

		Mat4 res; // = Zero;
		res.m[ 0] = x*x*C + c;
		res.m[ 1] = x*y*C - z*s;
		res.m[ 2] = x*z*C + y*s;
		res.m[ 3] = 0.f;

		res.m[ 4] = y*x*C + z*s;
		res.m[ 5] = y*y*C + c;
		res.m[ 6] = y*z*C - x*s;
		res.m[ 7] = 0.f;

		res.m[ 8] = z*x*C - y*s;
		res.m[ 9] = z*y*C + x*s;
		res.m[10] = z*z*C + c;
		res.m[11] = 0.f;

		res.m[12] = 0.f;
		res.m[13] = 0.f;
		res.m[14] = 0.f;
		res.m[15] = 1.f;
		
		return res;
	}
	
};

} // Math
} // Pxf

#endif // __PXF_MATH_MATRIX_H__
