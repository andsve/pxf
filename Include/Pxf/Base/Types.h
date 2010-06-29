#ifndef __PXF_BASE_TYPES_H__
#define __PXF_BASE_TYPES_H__

#include <Pxf/Base/Config.h>

// NO NAMESPACE
typedef unsigned int uint;


#if defined(CONF_COMPILER_GCC)
    typedef unsigned char uint8;
    typedef unsigned short uint16;
#ifndef _UINT32
    typedef unsigned long uint32; // fails on osx: /System/Library/Frameworks/Security.framework/Headers/cssmconfig.h:69: error: conflicting declaration ‘typedef uint32_t uint32’
#define _UINT32
#endif
    typedef short int16;
	typedef signed int int32;
    //typedef long int32;
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

