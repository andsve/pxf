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
	Pxf::Graphics::WindowSpecifications* pWindowSpecs = new Pxf::Graphics::WindowSpecifications();
	pWindowSpecs->Width = 720;
	pWindowSpecs->Height = 480;
	pWindowSpecs->ColorBits = 8;
	pWindowSpecs->AlphaBits = 0;
	pWindowSpecs->DepthBits = 24;
	pWindowSpecs->StencilBits = 0;
	pWindowSpecs->VerticalSync = false;
	pWindowSpecs->FSAASamples = 0;
	pWindowSpecs->Fullscreen = false;
	pWindowSpecs->Resizeable = false;
	Graphics::Device* pDevice = engine.CreateDevice(Graphics::EOpenGL2);
	//Graphics::Device* pDevice = engine.CreateDevice(Graphics::EDirect3D9);
	Graphics::Window* pWindow = pDevice->OpenWindow(pWindowSpecs);
	
	//Sleep(1000);
	pDevice->CloseWindow();
	engine.DestroyDevice(pDevice);	

	return true;
}