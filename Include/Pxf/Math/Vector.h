#ifndef __PXF_MATH_VECTOR_H__
#define __PXF_MATH_VECTOR_H__

#if defined(__APPLE__) || defined(__APPLE_CC__)
	#include <math.h> // sqrtf
#else
	#include <cmath> // sqrtf
#endif

// ASSERT(sizeof(Vec2f) == 2)
// ASSERT(sizeof(Vec3f) == 3)
// ASSERT(sizeof(Vec4f) == 4)

namespace Pxf {
namespace Math {

	//
	//	Vector2D
	//
	template <typename T>
	class Vector2D
	{
	public:
		union { T x, u; };
		union { T y, v; };

		Vector2D()     { x = y = 0; }
		Vector2D(T nv) { x = y = nv; }
		Vector2D(T nx, T ny) { x = nx; y = ny; }
		Vector2D(const Vector2D<T>& o) { *this = o; }
		
		inline void Set(T nx, T ny) { x = nx; y = ny; }

		Vector2D operator - () const { return Vector2D(-x, -y); }
		
		Vector2D operator - (const Vector2D &v) const { return Vector2D(x - v.x, y - v.y); }
		Vector2D operator + (const Vector2D &v) const { return Vector2D(x + v.x, y + v.y); }
		Vector2D operator * (const Vector2D &v) const { return Vector2D(x * v.x, y * v.y); }
		Vector2D operator / (const Vector2D &v) const { return Vector2D(x / v.x, y / v.y); }

		Vector2D operator - (const T t) const { return Vector2D(x - t, y - t); }
		Vector2D operator + (const T t) const { return Vector2D(x + t, y + t); }
		Vector2D operator * (const T t) const { return Vector2D(x * t, y * t); }
		Vector2D operator / (const T t) const { return Vector2D(x / t, y / t); }

		const Vector2D &operator -= (const Vector2D &v) { x -= v.x; y -= v.y; return *this; }
		const Vector2D &operator += (const Vector2D &v) { x += v.x; y += v.y; return *this; }
		const Vector2D &operator *= (const Vector2D &v) { x *= v.x; y *= v.y; return *this; }
		const Vector2D &operator /= (const Vector2D &v) { x /= v.x; y /= v.y; return *this; }

		const Vector2D &operator -= (const T t) { x -= t; y -= t; return *this; }
		const Vector2D &operator += (const T t) { x += t; y += t; return *this; }
		const Vector2D &operator *= (const T t) { x *= t; y *= t; return *this; }
		const Vector2D &operator /= (const T t) { x /= t; y /= t; return *this; }

		bool operator == (const Vector2D &v) const { return ((v.x == x) && (v.y == y)); }
		bool operator != (const Vector2D &v) const { return !(*this == v); }

		const Vector2D &operator = (const Vector2D &v) { x = v.x; y = v.y; return *this; }
	};

	template <typename T>
	inline T Dot(const Vector2D<T> &a, const Vector2D<T> &b)
	{ 
		return a.x*b.x + a.y*b.y;
	}

	template <typename T>
	inline T LengthSqr(const Vector2D<T> &a)
	{
		return a.x*a.x + a.y*a.y; 
	}

	template <typename T>
	inline T Length(const Vector2D<T> &a)
	{ 
		return sqrtf(LengthSqr(a));
	}
	
	template <typename T>
	inline T Distance(const Vector2D<T> &a, const Vector2D<T> &b)
	{ 
		return Length(a-b); 
	}
	
	template <typename T>
	inline Vector2D<T> Perpendicular(const Vector2D<T> &a)
	{ 
		return Vector2D<T>(a.y, -a.x);
	}

	template <typename T>
	inline void Clamp(const Vector2D<T> &a, T min, T max)
	{ 
		a.x = Clamp(a.x, min, max);
		a.y = Clamp(a.y, min, max);
	}

	template <typename T>
	inline Vector2D<T> Clamped(const Vector2D<T> &a, T min, T max)
	{ 
		Vector2D<T> ret = a;
		Clamp<T>(ret);
		return ret;
	}

	template <typename T>
	inline void Normalize(Vector2D<T> &a)
	{
		T len = Length(a);
		if (len > 0.0000001f)
		{
			a.x /= len;
			a.y /= len;
		}
		else
			a.x = a.y = 0;
	}

	template <typename T>
	inline Vector2D<T> Normalized(const Vector2D<T> &a)
	{
		Vector2D<T> ret = a;
		Normalize<T>(ret);
		return ret;
	}

	template <typename T>
	void Rotate(const Vector2D<T> &center, Vector2D<T> &point, float angle)
	{
		Vector2D<T> p = point - center;
		point.x = p.x * cosf(angle) - p.y * sinf(angle) + center.x;
		point.y = p.x * sinf(angle) + p.y * cosf(angle) + center.y;
	}

	//
	// Vector3D
	//
	template <typename T>
	class Vector3D
	{
	public:
		union { T x, r; };
		union { T y, g; };
		union { T z, b; };
	
		Vector3D()     { x = y = z = 0; }
		Vector3D(T nv) { x = y = z = nv; }
		Vector3D(T nx, T ny, T nz) { x = nx; y = ny; z = nz; }
		Vector3D(const Vector3D<T>& o) { *this = o; }

		inline void Set(T nx, T ny, T nz) { x = nx; y = ny; z = nz; }

		Vector3D operator - () const { return Vector3D(-x, -y, -z); }
		
		Vector3D operator - (const Vector3D &v) const { return Vector3D(x - v.x, y - v.y, z - v.z); }
		Vector3D operator + (const Vector3D &v) const { return Vector3D(x + v.x, y + v.y, z + v.z); }
		Vector3D operator * (const Vector3D &v) const { return Vector3D(x * v.x, y * v.y, z * v.z); }
		Vector3D operator / (const Vector3D &v) const { return Vector3D(x / v.x, y / v.y, z / v.z); }

		Vector3D operator - (const T t) const { return Vector3D(x - t, y - t, z - t); }
		Vector3D operator + (const T t) const { return Vector3D(x + t, y + t, z + t); }
		Vector3D operator * (const T t) const { return Vector3D(x * t, y * t, z * t); }
		Vector3D operator / (const T t) const { return Vector3D(x / t, y / t, z / t); }

		const Vector3D &operator -= (const Vector3D &v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
		const Vector3D &operator += (const Vector3D &v) { x += v.x; y += v.y; z += v.z; return *this; }
		const Vector3D &operator *= (const Vector3D &v) { x *= v.x; y *= v.y; z *= v.z; return *this; }
		const Vector3D &operator /= (const Vector3D &v) { x /= v.x; y /= v.y; z /= v.z; return *this; }

		const Vector3D &operator -= (const T t) { x -= t; y -= t; z -= t; return *this; }
		const Vector3D &operator += (const T t) { x += t; y += t; z += t; return *this; }
		const Vector3D &operator *= (const T t) { x *= t; y *= t; z *= t; return *this; }
		const Vector3D &operator /= (const T t) { x /= t; y /= t; z /= t; return *this; }

		bool operator == (const Vector3D &v) const { return ((v.x == x) && (v.y == y) && (v.z == z)); }
		bool operator != (const Vector3D &v) const { return !(*this == v); }

		const Vector3D &operator = (const Vector3D &v) { x = v.x; y = v.y; z = v.z; return *this; }
	};

	template <typename T>
	inline T Dot(const Vector3D<T> &a, const Vector3D<T> &b)
	{ 
		return a.x*b.x + a.y*b.y + a.z*b.z; 
	}

	template <typename T>
	inline T LengthSqr(const Vector3D<T> &a) 
	{ 
		return a.x*a.x + a.y*a.y + a.z*a.z; 
	}

	template <typename T>
	inline T Length(const Vector3D<T> &a) 
	{ 
		return sqrtf(LengthSqr(a)); 
	}
	
	template <typename T>
	inline T Distance(const Vector3D<T> &a, const Vector3D<T> &b) 
	{ 
		return Length(a-b);
	}
	
	template <typename T>
	inline void Clamp(const Vector3D<T> &a, T min, T max)
	{ 
		a.x = Clamp(a.x, min, max); 
		a.y = Clamp(a.y, min, max); 
		a.z = Clamp(a.z, min, max);
	}
	
	template <typename T>
	inline Vector3D<T> Clamped(const Vector3D<T> &a, T min, T max)
	{ 
		Vector3D<T> ret = a;
		Clamp<T>(ret);
		return ret;
	}

	template <typename T>
	inline void Normalize(Vector3D<T> &a)
	{
		T len = Length(a);
		if (len > 0.0000001f)
		{
			a.x /= len;
			a.y /= len;
			a.z /= len;
		}
		else
			a.x = a.y = a.z = 0;
	}

	template <typename T>
	inline Vector3D<T> Normalized(const Vector3D<T> &a)
	{
		Vector3D<T> ret = a;
		Normalize(ret);
		return ret;
	}

	template <typename T>
	inline Vector3D<T> Cross(const Vector3D<T> &a, const Vector3D<T> &b)
	{
		return Vector3D<T>( a.y * b.z - a.z * b.y, 
						    a.z * b.x - a.x * b.z,
						    a.x * b.y - a.y * b.x);
	}

	//
	// Vector4D
	// What was this for? Quaternions? Already have that in a separate class. kekeke.
	//
	template <typename T>
	class Vector4D
	{
	public:
		union { T x; T r; };
		union { T y; T g; };
		union { T z; T b; };
		union { T w; T a; };
	
		Vector4D() { x = y = z = w = 0; }
		Vector4D(T nv) { x = y = z = w = nv; }
		Vector4D(T nx, T ny, T nz, T nw) { x = nx; y = ny; z = nz; w = nw; }
		Vector4D(const Vector4D<T>& o) { *this = o; }

		inline void Set(T nx, T ny, T nz, T nw) { x = nx; y = ny; z = nz; w = nw; }

		Vector4D operator - () const { return Vector4D(-x, -y, -z, -w); }
		
		Vector4D operator - (const Vector4D &v) const { return Vector4D(x - v.x, y - v.y, z - v.z, w - v.w); }
		Vector4D operator + (const Vector4D &v) const { return Vector4D(x + v.x, y + v.y, z + v.z, w + v.w); }
		Vector4D operator * (const Vector4D &v) const { return Vector4D(x * v.x, y * v.y, z * v.z, w * v.w); }
		Vector4D operator / (const Vector4D &v) const { return Vector4D(x / v.x, y / v.y, z / v.z, w / v.w); }

		Vector4D operator - (const T t) const { return Vector4D(x - t, y - t, z - t, w - t); }
		Vector4D operator + (const T t) const { return Vector4D(x + t, y + t, z + t, w + t); }
		Vector4D operator * (const T t) const { return Vector4D(x * t, y * t, z * t, w * t); }
		Vector4D operator / (const T t) const { return Vector4D(x / t, y / t, z / t, w / t); }

		const Vector4D &operator -= (const Vector4D &v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
		const Vector4D &operator += (const Vector4D &v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
		const Vector4D &operator *= (const Vector4D &v) { x *= v.x; y *= v.y; z *= v.z; w *= v.w; return *this; }
		const Vector4D &operator /= (const Vector4D &v) { x /= v.x; y /= v.y; z /= v.z; w /= v.w; return *this; }

		const Vector4D &operator -= (const T t) { x -= t; y -= t; z -= t; w -= t; return *this; }
		const Vector4D &operator += (const T t) { x += t; y += t; z += t; w += t; return *this; }
		const Vector4D &operator *= (const T t) { x *= t; y *= t; z *= t; w *= t; return *this; }
		const Vector4D &operator /= (const T t) { x /= t; y /= t; z /= t; w /= t; return *this; }

		bool operator == (const Vector4D &v) const { return ((v.x == x) && (v.y == y) && (v.z == z) && (v.w == w)); }
		bool operator != (const Vector4D &v) const { return !(*this == v); }

		const Vector4D &operator = (const Vector4D &v) { x = v.x; y = v.y; z = v.z; w = v.w; return *this; }
	};

	template <typename T>
	inline T Dot(const Vector4D<T> &a, const Vector4D<T> &b)
	{ 
		return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
	}
	
	template <typename T>
	inline void Clamp(const Vector4D<T> &a, T min, T max)
	{ 
		a.x = Clamp(a.x, min, max);
		a.y = Clamp(a.y, min, max); 
		a.z = Clamp(a.z, min, max); 
		a.w = Clamp(a.w, min, max);
	}

	template <typename T>
	inline Vector4D<T> Clamped(const Vector4D<T> &a, T min, T max)
	{ 
		Vector4D<T> ret = a;
		Clamp(ret);
		return ret;
	}

	template <typename T>
	inline T LengthSqr(const Vector4D<T> &a)
	{ 
		return a.x*a.x + a.y*a.y + a.z*a.z + a.w*a.w;
	}

	template <typename T>
	inline T Length(const Vector4D<T> &a)
	{ 
		return sqrtf(LengthSqr(a)); 
	}
	
	template <typename T>
	inline void Normalize(Vector4D<T> &a)
	{
		T len = Length(a);
		if (len > 0.0000001f)
		{
			a.x /= len;
			a.y /= len;
			a.z /= len;
			a.w /= len;
		}
		else
			a.x = a.y = a.z = a.w = 0;
	}

	template <typename T>
	inline Vector4D<T> Normalized(const Vector4D<T> &a)
	{
		Vector4D<T> ret = a;
		Normalize(ret);
		return ret;
	}

	typedef Vector2D<float> Vec2f;
	typedef Vector3D<float> Vec3f;
	typedef Vector4D<float> Vec4f;

	typedef Vector2D<int> Vec2i;
	typedef Vector3D<int> Vec3i;
	typedef Vector4D<int> Vec4i;

} // Math
} // Pxf

#endif // __PXF_MATH_VECTOR_H__
