#ifndef _PXF_GRAPHICS_OPENGL_H_
#define _PXF_GRAPHICS_OPENGL_H_

#include <Pxf/Base/Config.h>	

	#if defined(CONF_FAMILY_WINDOWS)
		#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
		#undef CreateFont // ...
	#endif

	#if defined(CONF_PLATFORM_MACOSX)
	
	    #if defined(__i386__) && !defined(TARGET_OS_IPHONEDEV)
	        #include <GL/glew.h>
    		#include <OpenGL/OpenGL.h>
    		#include <OpenGL/gl.h>
    		#include <OpenGL/glu.h>
    		#include <GL/glfw.h>
	    #else
	        // Running un iPhone device or simulator
        	#include <OpenGLES/ES1/gl.h>
        	#include <OpenGLES/ES1/glext.h>
	        
	    #endif
	#else
	    #include <GL/glew.h>
		#include <GL/gl.h>
		#include <GL/glu.h>
		#include <GL/glfw.h>
	#endif

#endif // _PXF_GRAPHICS_OPENGL_H_
