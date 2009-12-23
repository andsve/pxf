#ifndef __PXF_BASE_TYPES_H__
#define __PXF_BASE_TYPES_H__

#include <Pxf/Base/Config.h>

// NO NAMESPACE

#if defined(CONF_COMPILER_GCC)
    typedef unsigned char uint8;
    typedef unsigned short uint16;
    typedef unsigned long uint32;
    typedef char int8;
    typedef short int16;
    typedef long int32;
    typedef unsigned long long uint64;
    typedef   signed long long int64;
#elif defined(CONF_COMPILER_MSVC)
    typedef unsigned __int8 uint8;
    typedef unsigned __int16 uint16;
    typedef unsigned __int32 uint32;
    typedef __int8 int8;
    typedef __int16 int16;
    typedef __int32 int32;
    typedef unsigned __int64 uint64;
    typedef   signed __int64  int64;
#else
#error "Compiler not supported"
#endif

typedef float fp32;
typedef double fp64;

#define pure = 0

#ifndef NULL
	#define NULL 0x0
#endif

#endif //__PXF_BASE_TYPES_H__

