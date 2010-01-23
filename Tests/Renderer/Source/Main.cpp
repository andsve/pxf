#include <Pxf/Pxf.h>
#include <Pxf/Engine.h>
#include <Pxf/Graphics/Device.h>
#include <Pxf/Graphics/OpenGL/DeviceGL2.h>
#include <Pxf/Graphics/DeviceType.h>
#include <Pxf/Graphics/Window.h>
#include <Pxf/Graphics/WindowSpecifications.h>
#include <Pxf/Graphics/QuadBatch.h>
#include <Pxf/Math/Math.h>
#include <Pxf/Input/Input.h>
#include <Pxf/Util/String.h>

using namespace Pxf;

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
	//Graphics::Device* pDevice = engine.CreateDevice(Graphics::EOpenGL3);
	Graphics::Window* pWindow = pDevice->OpenWindow(pWindowSpecs);
	Input::Input* pInput = engine.CreateInput(pDevice, pWindow);

	// Lets render some quads, but in "reverse" order via SetDepth(...).
	Graphics::QuadBatch* pQBatch = pDevice->CreateQuadBatch(256);
	pQBatch->Reset();
	pQBatch->SetDepth(0.5f);
	pQBatch->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
	pQBatch->AddCentered(200, 200, 50, 50);

	pQBatch->SetDepth(0.1f);
	pQBatch->SetColor(0.0f, 1.0f, 0.0f, 1.0f);
	pQBatch->AddCentered(225, 225, 50, 50);

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	glOrtho (0, pWindowSpecs->Width, pWindowSpecs->Height, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.375, 0.375, 0);

	float t_honk = 0.0f;
	while (!pInput->IsKeyDown(Input::ESC))
	{
		// Update input
		pInput->Update();

		glClearColor(.3, .3, .3, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		glTranslatef(0.375, 0.375, 0);

		t_honk += 0.001f;
		glTranslatef(cosf(t_honk) * 100.0f, sinf(t_honk) * 100.0f, 0);
		pQBatch->Draw();

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