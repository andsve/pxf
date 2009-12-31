#ifndef __PXF_BASE_UTILS_H__
#define __PXF_BASE_UTILS_H__

#include <Pxf/Base/Types.h>
#include <Pxf/Base/Config.h>

namespace Pxf
{

	// safe deletion of ptr
	template<typename T> 
	inline void SafeDelete(T& _Ptr)
	{
		if(_Ptr != 0) 
		{ 
			delete _Ptr;
			_Ptr = 0;
		}
	}

	// safe deletion of array
	template<typename T> 
	inline void SafeDeleteArray(T& _Ptr)
	{
		if(_Ptr != 0)
		{
			delete [] _Ptr;
			_Ptr = 0;
		}
	}

	//	void f(char *foo, ...)
	//		char buff[100]
	//		formatArgumentList(buff, &foo)
	// TODO inline these?
	int FormatArgumentList(char* _Dest, const char** _Format);

	// format(buff, "%s", "aoeu")
	int Format(char* _Dest, const char* _Format, ...);

	// Swaps the content of a and b
	template <typename T> 
	inline void Swap(T& a, T& b)
	{
		T t; t = a; a = b; b = t;
	}

	// Reverse the bits of a byte
	inline uint8 BitReverse8(uint8 v)
	{
		return (uint8)(((v * 0x0802LU & 0x22110LU) | (v * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16);
	}

	// Merge two bytes into a short
	inline uint16 Merge2(char a, char b)
	{
		return (a | (b << 8));
	}

	// Merge four bytes into a long
	inline uint32  Merge4(char a, char b, char c, char d)
	{
		return (a | (b << 8) | (c << 16) | (d << 24));
	}

	// Checks if an integer v is a power of 2
	inline bool IsPow2(uint32 v)
	{
		return (v & (v-1)) == 0;
	}

	// Checks if an integer v is even
	inline bool IsEven(uint32 v)
	{
		return !(v & 1);
	}

	// Swap two bytes
	inline uint16 Byteswap16(uint16 v)
	{
		return (v >> 8) | ((v & 0xFF) << 8);
	}

	// Swap four bytes
	inline uint32 Byteswap32(uint32 v)
	{
		return ((v>>24) | ((v&0xFF0000)>>8) | ((v&0xFF00)<<8) | ((v&0xFF)<<24));
	}

	// Swaps endianess for arbitrary data type 
	template<typename T> 
	static T ByteswapArb(const T& t)
	{
		if (sizeof(T) > 1)
		{
			T ret;
			char* rptr = (char*)&t + sizeof(T) - 1;
			char* wptr = (char*)&ret;
			for(uint len = sizeof(T); len > 0; len--)
			{
				*wptr = *rptr;
				wptr++; rptr--;
			}
			return ret;
		}
		return t;
	}

	// Set up routines for swapping from/to "neutral", little and big endian
	#if defined (CONF_CPU_ENDIAN_LITTLE)
		template <typename T> inline T ByteswapBtoN(const T& t) { return ByteswapArb(t); }
		template <typename T> inline T ByteswapNtoB(const T& t) { return ByteswapArb(t); }
		template <typename T> inline T ByteswapLtoN(const T& t) { return t; }
		template <typename T> inline T ByteswapNtoL(const T& t) { return t; }
	#elif defined (CONF_CPU_ENDIAN_BIG)
		template <typename T> inline T ByteswapBtoN(const T& t) { return t; }
		template <typename T> inline T ByteswapNtoB(const T& t) { return t; }
		template <typename T> inline T ByteswapLtoN(const T& t) { return ByteswapArb(t); }
		template <typename T> inline T ByteswapNtoL(const T& t) { return ByteswapArb(t); }
	#else
		#error unable to generate bitswapping functions for your platform. endianess unknown.
	#endif
	
} // namespace Pxf

#endif //__PXF_BASE_UTILS_H__

