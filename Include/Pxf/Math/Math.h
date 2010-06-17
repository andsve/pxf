#ifndef __PXF_MATH_MATH_H__
#define __PXF_MATH_MATH_H__

#if defined(__APPLE__) || defined(__APPLE_CC__)
	#include <math.h>
#else
	#include <cmath>
#endif

namespace Pxf {
namespace Math {

const float PI = 3.1415926535897932384626f;

inline float Deg2Rad(float deg)
{
	return (deg * PI) / 180.0f;
}

inline float Rad2Deg(float rad)
{
	return (rad * 180.0f) / PI;
}

inline bool nearValue(float t, float value)
{
	return fabsf((t-value) / ((value == 0.0f) ? 1.0f : value)) < 1e-6f;
}

// Transform value from one range to another range
template <typename T>
inline T TransformRange(T value, T in_min, T in_max, T out_min, T out_max)
{
	float rel = (value-in_min) / static_cast<float>(in_max-in_min);
	return static_cast<T>(rel*(out_max-out_min)+out_min);
}

// Returns the absolute value of a
template <typename T> 
inline T Abs(const T& a)
{
	return (a < 0 ? -a : a);
}

// Returns the biggest of a and b
template<typename T> 
inline T Max(const T& a, const T& b)
{
	return (b < a ? a : b);
}

// Returns the smallest of a and b
template<typename T> 
inline T Min(const T& a,const T& b)
{
	return (a < b ? a : b);
}

// Clamps value between min and max
template <typename T> 
inline T Clamp(const T& value, const T& min, const T& max)
{
	return (value < min ? min : (value > max ? max : value));
}

} // Math
} // Pxf

#endif // __PXF_MATH_MATH_H__
