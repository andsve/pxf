#include <Pxf/Pxf.h>
#include <Pxf/Engine.h>
#include <Pxf/Graphics/Device.h>
#include <Pxf/Graphics/OpenGL/DeviceGL2.h>
#include <Pxf/Graphics/DeviceType.h>
#include <Pxf/Graphics/Window.h>
#include <Pxf/Graphics/WindowSpecifications.h>
#include <Pxf/Input/Input.h>
#include <Pxf/Util/String.h>

#include <Pxf/Extra/SimpleNet/SimpleNet.h>
#include <Pxf/Extra/SimpleNet/SimpleClient.h>
#include <Pxf/Extra/SimpleNet/SimpleServer.h>
#include <Pxf/Extra/SimpleFont/SimpleFont.h>
#include <Pxf/Extra/LuaGUI/LuaGUI.h>
#include <Pxf/Extra/LuaGUI/GUIHandler.h>

//#define IS_SERVER 1


using namespace Pxf;
using namespace Pxf::Graphics;
using namespace Pxf::Extra;
using namespace Pxf::Extra::LuaGUI;

bool PxfMain(Util::String _CmdLine)
{
	char t_title[512];
#ifdef IS_SERVER
	char t_pxftitle[] = "Network Test Server";
#else
	char t_pxftitle[] = "Network Test Client";
#endif
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
  
  // Setup engine/device/window
	Pxf::Engine engine;
	Graphics::Device* pDevice = engine.CreateDevice(Graphics::EOpenGL2);
	Graphics::Window* pWindow = pDevice->OpenWindow(pWindowSpecs);
	Input::Input* pInput = engine.CreateInput(pDevice, pWindow);
	pInput->ShowCursor(true);

  // Setup testgui
	GUIHandler* pGUI = new GUIHandler(pDevice);
	pGUI->AddScript("data/guitest.lua", &Pxf::Math::Vec4i(0,0,300,pWindowSpecs->Height)); // Fix this? viewport seems to be set from bottom left corner?

	// Setup viewport and orthogonal projection
	pDevice->SetViewport(0, 0, pWindowSpecs->Width / 2.0f, pWindowSpecs->Height);
	Math::Mat4 t_ortho = Math::Mat4::Ortho(0, pWindowSpecs->Width / 2.0f, pWindowSpecs->Height, 0, 0, 1);
	pDevice->SetProjection(&t_ortho);
	
	// Test font
	/*SimpleFont *_fonttest = new SimpleFont(pDevice);
	_fonttest->Load("data/alterebro_pixel.ttf", 13.0f, 128);
	_fonttest->AddTextCentered("Hey, some text! :)", Math::Vec3f(0,10,0));*/
	
	// Setup networking
#ifdef IS_SERVER
  SimpleServer *pNet = new SimpleServer(NULL, 4632);
  pNet->Open();
#else
  SimpleClient *pNet = new SimpleClient();
  pNet->Connect("localhost", 4632);
#endif

	while (!pInput->IsKeyDown(Input::ESC) && pWindow->IsOpen())
	{
		
		// Some OGL stuff that hasn't been moved to the device yet
		pDevice->SetViewport(0, 0, pWindowSpecs->Width, pWindowSpecs->Height);
		pDevice->SetProjection(&t_ortho);
		glClearColor(.3, .3, .3, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		glEnable(GL_TEXTURE_2D);

    // Update network
    pNet->MessagePump();

		// Update input
		pInput->Update();

    // Store mouse input for GUI
		Math::Vec2i mousepos_i;
		Math::Vec2f mousepos_f;
		pInput->GetMousePos(&mousepos_i.x, &mousepos_i.y);
		mousepos_f.x = mousepos_i.x;
		mousepos_f.y = mousepos_i.y;

		// GUI
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		pGUI->Update(&mousepos_f, pInput->IsButtonDown(Pxf::Input::MOUSE_LEFT), 1.0f);
		pGUI->Draw();

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
	
  delete pNet;

	pDevice->CloseWindow();
	engine.DestroyDevice(pDevice);

	return true;
}