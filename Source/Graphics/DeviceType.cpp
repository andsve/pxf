#include <Pxf/Graphics/DeviceType.h>
#include <Pxf/Base/Debug.h>

using namespace Pxf;

const char* Graphics::DeviceTypeName(Graphics::DeviceType _deviceType)
{
	switch(_deviceType)
	{
		case EOpenGL2: return "OpenGL 2.x";
		case EOpenGL3: return "OpenGL 3.x";
		case EOpenGLES11: return "OpenGL ES 1.1";
		case EOPENGLES2: return "OpenGL ES 2.x";
		//case EDirect3D9: return "Direct3D 9";
		//case EDirect3D10: return "Direct3D 10";
		//case EDirect3D11: return "Direct3D 11";
		default: PXFASSERT(0, "Invalid device type!");
	}
	return 0;
}

