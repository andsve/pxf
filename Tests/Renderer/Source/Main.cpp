#include <Pxf/Pxf.h>
#include <Pxf/Engine.h>
#include <Pxf/Graphics/Device.h>
#include <Pxf/Graphics/OpenGL/DeviceGL2.h>
#include <Pxf/Graphics/DeviceType.h>
#include <Pxf/Graphics/Window.h>
#include <Pxf/Graphics/WindowSpecifications.h>
#include <Pxf/Graphics/Texture.h>
#include <Pxf/Graphics/QuadBatch.h>
#include <Pxf/Math/Math.h>
#include <Pxf/Math/Matrix.h>
#include <Pxf/Input/Input.h>
#include <Pxf/Util/String.h>
#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/Image.h>
#include <Pxf/Resource/Chunk.h>

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
	Graphics::Window* pWindow = pDevice->OpenWindow(pWindowSpecs);
	Input::Input* pInput = engine.CreateInput(pDevice, pWindow);

	Resource::ResourceManager* resourceManager = new Resource::ResourceManager();
	Resource::Image* image = resourceManager->Acquire<Resource::Image>("test.png");

	// Load some texture
	glEnable(GL_TEXTURE_2D);
	Graphics::Texture* pTexture = pDevice->CreateTexture("test.png");
	pDevice->BindTexture(pTexture);

	// Lets create some quads, but render them in "reverse" order via SetDepth(...).
	Graphics::QuadBatch* pQBatch = pDevice->CreateQuadBatch(256);
	pQBatch->Reset();
	pQBatch->SetTextureSubset(0.0f, 0.0f, 32.0f / pTexture->GetWidth(), 32.0f / pTexture->GetHeight());
	pQBatch->SetDepth(0.5f);
	pQBatch->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
	pQBatch->AddCentered(200, 200, 50, 50);
	pQBatch->SetDepth(0.1f);
	pQBatch->SetColor(0.0f, 1.0f, 0.0f, 1.0f);
	pQBatch->AddCentered(225, 225, 50, 50);

	// Setup viewport and orthogonal projection
	pDevice->SetViewport(0, 0, pWindowSpecs->Width / 2.0f, pWindowSpecs->Height);
	Math::Mat4 t_ortho = Math::Mat4::Ortho(0, pWindowSpecs->Width / 2.0f, pWindowSpecs->Height, 0, 0, 1);
	pDevice->SetProjection(&t_ortho);

	float t_honk = 0.0f;
	while (!pInput->IsKeyDown(Input::ESC) && pWindow->IsOpen())
	{
		// Update input
		pInput->Update();

		// Some OGL stuff that hasn't been moved to the device yet
		glClearColor((((int)(t_honk * 255)) % 255) / 255.0f,
			         .3, .3, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		glTranslatef(0.375, 0.375, 0);

		t_honk += 0.001f;
		glTranslatef(cosf(t_honk) * 200.0f, sinf(t_honk) * 200.0f, 0);
		
		pDevice->BindTexture(pTexture);
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