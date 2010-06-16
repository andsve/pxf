#ifndef _PXF_GRAPHICS_OPENGL_H_
#define _PXF_GRAPHICS_OPENGL_H_

#include <Pxf/Base/Config.h>

#if defined(CONF_PLATFORM_MACOSX) 
	#include "TargetConditionals.h"
	#if defined(TARGET_OS_IPHONE) || defined(TARGET_SIMULATOR_IPHONE)
		// need something for ES2 aswell :(
		#include <OpenGLES/ES1/gl.h>
		#include <OpenGLES/ES1/glext.h>
	#endif
#else
	#include <GL/glew.h>

	#if defined(CONF_FAMILY_WINDOWS)
		#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
		#undef CreateFont // ...
	#endif

	#if defined(CONF_PLATFORM_MACOSX)
		#include <OpenGL/OpenGL.h>
		#include <OpenGL/gl.h>
		#include <OpenGL/glu.h>
	#else
		#include <GL/gl.h>
		#include <GL/glu.h>
	#endif

	#include <GL/glfw.h>
#endif

#endif // _PXF_GRAPHICS_OPENGL_H_
