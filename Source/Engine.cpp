#include <Pxf/Pxf.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Engine.h>
#include <Pxf/Graphics/DeviceType.h>
//#include <Pxf/Graphics/Device.h> // replace with OGL and D3D device headers
#include <Pxf/Graphics/D3D9/DeviceD3D9.h>
#include <Pxf/Graphics/OpenGL/DeviceGL2.h>
#include <Pxf/Graphics/OpenGL/DeviceGL3.h>

using namespace Pxf;

Graphics::Device* Engine::CreateDevice(Graphics::DeviceType _deviceType)
{
	switch(_deviceType)
	{
	case Graphics::EOpenGL2: return new Graphics::DeviceGL2();
	case Graphics::EOpenGL3: return new Graphics::DeviceGL3();
	case Graphics::EDirect3D9: return new Graphics::DeviceD3D9();
	
	default:
		PXFASSERT(0, "Chosen device type is not available.");
	}
	
	return NULL;
}

void Engine::DestroyDevice(Graphics::Device* _pDevice)
{
	if (_pDevice)
		delete _pDevice;
}