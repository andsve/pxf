#include <Pxf/Graphics/OpenGL/DeviceGLES11.h>
#include <Pxf/Base/Debug.h>

using namespace Pxf;
using namespace Graphics;

void DeviceGLES11::CreateRenderBuffer(RenderBufferGL& _Buffer,int _Width, int _Height, int _Format)
{

}

void DeviceGLES11::SwapBuffers()
{
	Message("Device","SwapBuffers");
}