#include <Pxf/Pxf.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Engine.h>
#include <Pxf/Graphics/DeviceType.h>
#include <Pxf/Graphics/Device.h> // replace with OGL and D3D device headers
#include <Pxf/Graphics/DeviceD3D9.h>

using namespace Pxf;

Graphics::Device* Engine::CreateDevice(Graphics::DeviceType _deviceType)
{
	switch(_deviceType)
	{
	case Graphics::EOpenGL2: return NULL; // new Device_OGL2();
	case Graphics::EDirect3D9: new DeviceD3D9();
	
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