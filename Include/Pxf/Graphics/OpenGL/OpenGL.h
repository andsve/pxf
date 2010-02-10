#ifndef _PXF_GRAPHICS_OPENGL_H_
#define _PXF_GRAPHICS_OPENGL_H_

#include <Pxf/Base/Config.h>

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



#endif // _PXF_GRAPHICS_OPENGL_H_
