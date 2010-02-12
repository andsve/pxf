#include <Pxf/Pxf.h>
#include <Pxf/Engine.h>
#include <Pxf/Graphics/Device.h>
#include <Pxf/Graphics/OpenGL/DeviceGL2.h>
#include <Pxf/Graphics/OpenGL/RenderTargetGL2.h>
#include <Pxf/Graphics/DeviceType.h>
#include <Pxf/Graphics/Window.h>
#include <Pxf/Graphics/WindowSpecifications.h>
#include <Pxf/Graphics/Texture.h>
#include <Pxf/Graphics/QuadBatch.h>
#include <Pxf/Math/Math.h>
#include <Pxf/Math/Matrix.h>
#include <Pxf/Input/Input.h>
#include <Pxf/Util/String.h>
#include <Pxf/Resource/ShaderSource.h>
#include <Pxf/Resource/Image.h>
#include <Pxf/Resource/Chunk.h>
#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/ShaderSource.h>

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

	Pxf::Resource::Image t_Image(new Pxf::Resource::Chunk(),"test.png");
	Pxf::Resource::ResourceManager* t_ResourceManager = new Pxf::Resource::ResourceManager();

	Pxf::Resource::ShaderSource* t_ShaderSrc = t_ResourceManager->Acquire<Pxf::Resource::ShaderSource>("shader_test.txt");

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
	pQBatch->AddCentered(300, 200, 50, 50);

	// Setup viewport and orthogonal projection
	pDevice->SetViewport(0, 0, pWindowSpecs->Width / 2.0f, pWindowSpecs->Height);
	Math::Mat4 t_ortho = Math::Mat4::Ortho(0, pWindowSpecs->Width / 2.0f, pWindowSpecs->Height, 0, 0, 1);
	pDevice->SetProjection(&t_ortho);

	Graphics::Texture* pRColorTex0 = pDevice->CreateEmptyTexture(200,200);
	Pxf::Graphics::RenderTarget* pRT0 = pDevice->CreateRenderTarget(pWindowSpecs->Width,pWindowSpecs->Height,Pxf::Graphics::RT_FORMAT_RGBA8,Pxf::Graphics::RT_FORMAT_DEPTH_COMPONENT);
	pRT0->AddColorAttachment(pRColorTex0);	

	while (!pInput->IsKeyDown(Input::ESC) && pWindow->IsOpen())
	{
		// Update input
		pInput->Update();

		pDevice->BindRenderTarget(pRT0);

		// Some OGL stuff that hasn't been moved to the device yet
		glClearColor(0.0f,0.0f,0.0f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		pDevice->BindTexture(pTexture);
		pQBatch->Draw();

		pDevice->ReleaseRenderTarget(pRT0);

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