#include <Pxf/Pxf.h>
#include <Pxf/Engine.h>
#include <Pxf/Graphics/Device.h>
#include <Pxf/Graphics/OpenGL/DeviceGL2.h>
#include <Pxf/Graphics/DeviceType.h>
#include <Pxf/Graphics/Window.h>
#include <Pxf/Graphics/WindowSpecifications.h>
#include <Pxf/Input/Input.h>
#include <Pxf/Util/String.h>

#include <Pxf/Extra/LuaGUI/LuaGUI.h>
#include <Pxf/Extra/LuaGUI/GUIHandler.h>

using namespace Pxf;
using namespace Pxf::Graphics;
using namespace Pxf::Extra::LuaGUI;

bool PxfMain(Util::String _CmdLine)
{
	char t_title[512];
	char t_pxftitle[] = "PXF Engine";
	int t_fps = 0;

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

	Pxf::Engine engine;
	Graphics::Device* pDevice = engine.CreateDevice(Graphics::EOpenGL2);
	Graphics::Window* pWindow = pDevice->OpenWindow(pWindowSpecs);
	Input::Input* pInput = engine.CreateInput(pDevice, pWindow);

	GUIHandler* pGUI = new GUIHandler("data/guilook.png", pDevice);
	pGUI->AddScript("data/guitest.lua", &Pxf::Math::Vec4i(0,0,256,256));

	// Setup viewport and orthogonal projection
	pDevice->SetViewport(0, 0, pWindowSpecs->Width / 2.0f, pWindowSpecs->Height);
	Math::Mat4 t_ortho = Math::Mat4::Ortho(0, pWindowSpecs->Width / 2.0f, pWindowSpecs->Height, 0, 0, 1);
	pDevice->SetProjection(&t_ortho);

	while (!pInput->IsKeyDown(Input::ESC) && pWindow->IsOpen())
	{
		
		// Some OGL stuff that hasn't been moved to the device yet
		glClearColor(.3, .3, .3, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		glTranslatef(0.375, 0.375, 0);
		glEnable(GL_TEXTURE_2D);
		pDevice->SetProjection(&t_ortho);

		// Update input
		pInput->Update();

		// GUI
		pGUI->Update(1.0f);
		pGUI->Draw();

		

		//glTranslatef(cosf(t_honk) * 200.0f, sinf(t_honk) * 200.0f, 0);

		/*pDevice->BindTexture(pTexture);
		pQBatch->Draw();
		*/

		// Swap buffers
		pWindow->Swap();

		// Update title with FPS
		if (t_fps != pWindow->GetFPS())
		{
			t_fps = pWindow->GetFPS();
			sprintf(t_title, "%s - %s - FPS: %i", t_pxftitle, Graphics::DeviceTypeName(pDevice->GetDeviceType()), t_fps);
			pWindow->SetTitle(t_title);
		}
	}

	pDevice->CloseWindow();
	engine.DestroyDevice(pDevice);

	return true;
}