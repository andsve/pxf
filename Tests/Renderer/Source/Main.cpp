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
#include <Pxf/Math/Vector.h>
#include <Pxf/Input/Input.h>
#include <Pxf/Util/String.h>
#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/Image.h>
#include <Pxf/Resource/Chunk.h>

#include <Pxf/Graphics/InterleavedVertexBuffer.h>

using namespace Pxf;
using Math::Vec3f;
using Math::Vec4f;

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
	Math::Mat4 t_ortho = Math::Mat4::Ortho(0, pWindowSpecs->Width / 2.0f, pWindowSpecs->Height, 0, -1000, 1000);
	pDevice->SetProjection(&t_ortho);

	Graphics::InterleavedVertexBuffer* pBuff = pDevice->CreateInterleavedVertexBuffer(Graphics::VB_LOCATION_SYS);
	
	struct MyVertex
	{
		Vec3f v;
		Vec4f c;
		MyVertex(Vec3f _v, Vec4f _c)
		{
			v = _v;
			c = _c;
		}
	};

	
	pBuff->CreateNewBuffer(24, sizeof(Vec3f) + sizeof(Vec4f), Graphics::VB_STATIC_DRAW);

	// SetData(Type, Offset, NumFloats)
	pBuff->SetData(Graphics::VB_VERTEX_DATA, 0, 3);
	pBuff->SetData(Graphics::VB_COLOR_DATA, sizeof(Vec3f), 4);
	//pBuff->SetPrimitive(PRIMITIVE_QUAD);

	MyVertex* data = (MyVertex*)pBuff->MapData(Graphics::VB_WRITE_ONLY);

	// Front
	data[0]  = MyVertex(Vec3f(-0.5f, -0.5f, 0.5f), Vec4f(0, 0, 1, 1.0f));
	data[1]  = MyVertex(Vec3f(0.5f, -0.5f, 0.5f), Vec4f(0, 0, 1, 1.0f));
	data[2]  = MyVertex(Vec3f(0.5f, 0.5f, 0.5f), Vec4f(0, 0, 1, 1.0f));
	data[3]  = MyVertex(Vec3f(-0.5f, 0.5f, 0.5f), Vec4f(0, 0, 1, 1.0f));
	// Back
	data[4]  = MyVertex(Vec3f(-0.5f, -0.5f, -0.5f), Vec4f(1, 0, 1, 1.0f));
	data[5]  = MyVertex(Vec3f(-0.5f, 0.5f, -0.5f), Vec4f(1, 0, 1, 1.0f));
	data[6]  = MyVertex(Vec3f(0.5f, 0.5f, -0.5f), Vec4f(1, 0, 1, 1.0f));
	data[7]  = MyVertex(Vec3f(0.5f, -0.5f, -0.5f), Vec4f(1, 0, 1, 1.0f));
	// Top
	data[8]  = MyVertex(Vec3f(-0.5f, 0.5f, 0.5f), Vec4f(0, 1, 1, 1.0f));
	data[9]  = MyVertex(Vec3f(0.5f, 0.5f, 0.5f), Vec4f(0, 1, 1, 1.0f));
	data[10] = MyVertex(Vec3f(0.5f, 0.5f, -0.5), Vec4f(0, 1, 1, 1.0f));
	data[11] = MyVertex(Vec3f(-0.5f, 0.5f, -0.5f), Vec4f(0, 1, 1, 1.0f));
	// Left
	data[12] = MyVertex(Vec3f(-0.5f, -0.5f, 0.5f), Vec4f(1, 1, 1, 1.0f));
	data[13] = MyVertex(Vec3f(-0.5f, 0.5f, 0.5f), Vec4f(1, 1, 1, 1.0f));
	data[14] = MyVertex(Vec3f(-0.5f, 0.5f, -0.5f), Vec4f(1, 1, 1, 1.0f));
	data[15] = MyVertex(Vec3f(-0.5f, -0.5f, -0.5f), Vec4f(1, 1, 1, 1.0f));
	// Right
	data[16] = MyVertex(Vec3f(0.5f, -0.5f, -0.5f), Vec4f(1, 0, 1, 1.0f));
	data[17] = MyVertex(Vec3f(0.5f, 0.5f, -0.5f), Vec4f(1, 0, 1, 1.0f));
	data[18] = MyVertex(Vec3f(0.5f, 0.5f, 0.5f), Vec4f(1, 0, 1, 1.0f));
	data[19] = MyVertex(Vec3f(0.5f, -0.5f, 0.5f), Vec4f(1, 0, 1, 1.0f));
	// Bottom
	data[20] = MyVertex(Vec3f(-0.5f, -0.5f, 0.5f), Vec4f(0, 1, 1, 1.0f));
	data[21] = MyVertex(Vec3f(-0.5f, -0.5f, -0.5f), Vec4f(0, 1, 1, 1.0f));
	data[22] = MyVertex(Vec3f(0.5f, -0.5f, -0.5f), Vec4f(0, 1, 1, 1.0f));
	data[23] = MyVertex(Vec3f(0.5f, -0.5f, 0.5f), Vec4f(0, 1, 1, 1.0f));

	pBuff->UnmapData();


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

		t_honk += 0.004;

		glEnable(GL_DEPTH_TEST);
		glBindTexture(GL_TEXTURE_2D, 0);
		glTranslatef(100, 100, 0);
		glRotatef(t_honk*80, 0, 1, 1);
		glScalef(50, 50, 50);

		pDevice->DrawBuffer(pBuff);

		//pDevice->BindTexture(pTexture);
		//pQBatch->Draw();

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