#ifndef _PXF_GRAPHICS_DEVICEDEFS_H
#define _PXF_GRAPHICS_DEVICEDEFS_H

#include <Pxf/Math/Math.h>
#include <Pxf/Math/Vector.h>
#include <Pxf/Math/Matrix.h>

namespace Pxf
{
	namespace Graphics
	{
		enum DeviceType
		{
			EOpenGL2,
			EOpenGL3,
			EOpenGLES11,
			EOpenGLES2,
			EDirect3D9,
			EDirect3D10,
			EDirect3D11
		};
	}
}

#endif