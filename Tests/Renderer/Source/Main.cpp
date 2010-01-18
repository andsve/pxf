#include <Pxf/Pxf.h>
#include <Pxf/Engine.h>
#include <Pxf/Graphics/Device.h>
#include <Pxf/Graphics/DeviceType.h>
#include <Pxf/Graphics/Window.h>
#include <Pxf/Graphics/WindowSpecifications.h>
#include <Pxf/Util/String.h>

using namespace Pxf;

bool PxfMain(Util::String _CmdLine)
{
	Pxf::Engine engine;
	Pxf::Graphics::WindowSpecifications* pWindowSpecs;
	Graphics::Device* pDevice = engine.CreateDevice(Graphics::EOpenGL2);
	Graphics::Window* pWindow = pDevice->CreateWindow(pWindowSpecs);
	
	pDevice->DestroyWindow(pWindow);
	engine.DestroyDevice(pDevice);	

	return true;
}