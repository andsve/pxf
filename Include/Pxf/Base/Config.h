#ifndef _PXF_BASE_CONFIG_H_
#define _PXF_BASE_CONFIG_H_

// NO NAMESPACE

// System family:
// -------------
// Windows
#	if defined (WIN64) || defined (_WIN64)
#		define CONF_FAMILY_WINDOWS
#		define CONF_FAMILY_STRING "windows"	
#		define CONF_PLATFORM_WIN64
#		define CONF_PLATFORM_STRING "win64"
#	elif defined (WIN32) || defined (_WIN32) || defined(__CYGWIN32__) || defined (__MINGW32__)
#		define CONF_FAMILY_WINDOWS
#		define CONF_FAMILY_STRING "windows"
#		define CONF_PLATFORM_WIN32
#		define CONF_PLATFORM_STRING "win32"

// UNIX
#	elif defined(__FreeBSD__)
#		define CONF_FAMILY_UNIX
#		define CONF_FAMILY_STRING "unix"
#		define CONF_PLATFORM_FREEBSD
#		define CONF_PLATFORM_STRING "freebsd"

#	elif defined (__LINUX__) || defined (__linux__)
#		define CONF_FAMILY_UNIX
#		define CONF_FAMILY_STRING "unix"
#		define CONF_PLATFORM_LINUX
#		define CONF_PLATFORM_STRING "linux"

#	elif defined (MACOSX) || defined (__APPLE__)
#		define CONF_FAMILY_UNIX
#		define CONF_FAMILY_STRING "unix"
#		define CONF_PLATFORM_MACOSX
#		define CONF_PLATFORM_STRING "macosx"

#	else
#		define CONF_FAMILY_UNKNOWN
#		define CONF_FAMILY_STRING "unknown"
#		define CONF_PLATFORM_UNKNOWN
#		define CONF_PLATFORM_STRING "unknown"
#	endif


//	Architectures:
//	-------------
#	if defined(i386) || defined(__i386__) || defined(__x86__) || defined(CONF_PLATFORM_WIN32)
#		define CONF_ARCH_IA32
#		define CONF_ARCH_STRING "ia32"
#		define CONF_CPU_ENDIAN_LITTLE
#	endif

#	if defined(__ia64__)
#		define CONF_ARCH_IA64
#		define CONF_ARCH_STRING "ia64"
#		define CONF_CPU_ENDIAN_LITTLE
#	endif

#	if defined(__amd64__) || defined(__x86_64__)
#		define CONF_ARCH_AMD64
#		define CONF_ARCH_STRING "amd64"
#		define CONF_CPU_ENDIAN_LITTLE
#	endif

#	if defined(__powerpc__) || defined(__ppc__)
#		define CONF_ARCH_PPC
#		define CONF_ARCH_STRING "ppc"
#		define CONF_CPU_ENDIAN_BIG
#	endif

// Compilers:
// ---------
#	if defined(_MSC_VER)
#		define CONF_COMPILER_MSVC
#		define CONF_COMPILER_STRING "msvc"
#		if _MSC_VER >= 1300
#			define CONF_COMPILER_MSVC_NET
#		else
#			define CONF_COMPILER_MSVC_OLD
#		endif

#	elif defined(__GNUC__)
#		define CONF_COMPILER_GCC
#		define CONF_COMPILER_STRING "gcc"

#	else
#		define CONF_COMPILER_UNKNOWN
#		define CONF_COMPILER_STRING "unknown"
#	endif

// Debug
#include<assert.h>
#	if defined(_DEBUG) && !defined(CONF_DEBUG)
#		define CONF_DEBUG
#   elif !defined(CONF_RELEASE)
#		define CONF_RELEASE
#	endif

// Disable warnings of depricated cstdlib-functions.
#	if defined(CONF_COMPILER_MSVC_NET)
#		pragma warning (disable: 4996)
#	endif

#endif //_PXF_BASE_CONFIG_H_
