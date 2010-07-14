

#include <Pxf/Pxf.h>
#include <Pxf/Engine.h>
#include <Pxf/Graphics/Device.h>
#include <Pxf/Graphics/OpenGL/DeviceGL2.h>
#include <Pxf/Graphics/DeviceType.h>
#include <Pxf/Graphics/Window.h>
#include <Pxf/Graphics/WindowSpecifications.h>
#include <Pxf/Input/Input.h>
#include <Pxf/Util/String.h>

#include <Pxf/Extra/LuaGame/LuaGame.h>


using namespace Pxf;
using namespace Pxf::Graphics;
using namespace Pxf::Extra::LuaGame;

bool PxfMain(Util::String _CmdLine)
{
	char t_title[512];
	char t_pxftitle[] = "PXF iPhone Fake/Simulator";
	int t_fps = 0;

	Pxf::Graphics::WindowSpecifications* pWindowSpecs = new Pxf::Graphics::WindowSpecifications();
	pWindowSpecs->Width = 720;
	pWindowSpecs->Height = 720;
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
	pInput->ShowCursor(true);
	
	// Load iPhone background
	QuadBatch* bg_qb = pDevice->CreateQuadBatch(128);
    Texture* bg = pDevice->CreateTexture("iphonebg.png");
	
	// Fix so we are in the data dir
    chdir("data/");
	
	// Setup LuaGame
	Game* luagame = new Game("game.lua", pDevice);
	luagame->Load();
	
	while (!pInput->IsKeyDown(Input::ESC) && pWindow->IsOpen())
	{
		
    	pDevice->SetViewport(0.0f, 0.0f, pWindowSpecs->Width, pWindowSpecs->Height);
    	Math::Mat4 t_ortho = Math::Mat4::Ortho(0, pWindowSpecs->Width, pWindowSpecs->Height, 0, 0, 1);
    	pDevice->SetProjection(&t_ortho);
    	
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_DEPTH_TEST);
    	glDepthFunc(GL_LEQUAL);
    	glEnable(GL_BLEND);
    	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Render iPhone bg
        bg_qb->Reset();
        
        // bg
        pDevice->BindTexture(bg);
        bg_qb->Translate(pWindowSpecs->Width / 2.0f, pWindowSpecs->Height / 2.0f);
        bg_qb->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
        bg_qb->SetTextureSubset(0.0f, 0.0f, 1.0f, 1.0f);
        bg_qb->SetDepth(-1);
        bg_qb->AddCentered(0, 0, 512, 1024);
        bg_qb->Draw();

		// Update input
		pInput->Update();

		Math::Vec2i mousepos_i;
		Math::Vec2f mousepos_f;
		pInput->GetMousePos(&mousepos_i.x, &mousepos_i.y);
		mousepos_f.x = mousepos_i.x;
		mousepos_f.y = mousepos_i.y;

		// LuaGame
		pDevice->SetViewport(pWindowSpecs->Width / 2.0f - 160, pWindowSpecs->Height / 2.0f - 240, 320, 480);
    	pDevice->SetProjection(&Math::Mat4::Ortho(0, 320, 480, 0, 0, 1));
    	luagame->Update(0.1);
    	luagame->Render();

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