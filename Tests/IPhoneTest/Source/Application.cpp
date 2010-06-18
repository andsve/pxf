/*
 *  Application.cpp
 *  project
 *
 *  Created by jhonny on 6/2/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "../Include/Application.h"
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#include <Pxf/Math/Vector.h>
#include <Pxf/Graphics/VertexBuffer.h>

using namespace Pxf;
using namespace Math;

Application::Application(const char* _Title)
	: m_Title(_Title),
	  m_Engine(0),
	  m_IsRunning(false)
{

}

Application::~Application()
{
	m_Title = 0;
	delete m_Engine;
}
	

bool Application::Update()
{
	bool _RetVal = true;
	// Call update on scene
	
	_UpdateFPS();

	
	return _RetVal;
}

void Application::_UpdateFPS()
{
	m_FPS.ticks++;
	
	if(m_FPS.elapsed_time >= 60.0f)
	{
		m_FPS.fps = m_FPS.ticks / m_FPS.elapsed_time;
		m_FPS.ticks = 0;
		m_FPS.elapsed_time = 0.0f;
	}	
}

bool Application::Render()
{
	bool _RetVal = true;
	// Call render on scene 
	
	const GLfloat squareVertices[] = {
        -0.5f, -0.5f,
        0.5f,  -0.5f,
        -0.5f,  0.5f,
        0.5f,   0.5f,
    };
	
	const GLfloat squareTexcoords[] = {
		0.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
	};
	
    const GLubyte squareColors[] = {
        255, 255,   0, 255,
        0,   255, 255, 255,
        0,     0,   0,   0,
        255,   0, 255, 255,
    };
	
	m_Device->BindTexture(pTexture);
	
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrthof(-1.0f, 1.0f, -1.5f, 1.5f, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glRotatef(3.0f, 0.0f, 0.0f, 1.0f);
    
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
	m_Device->DrawBuffer(pBuffer);
	
	return _RetVal;
}

void Application::SetDevice(Pxf::Graphics::Device* _pDevice)
{
	m_Device = _pDevice;
	
	Setup();
}

struct MyVertex
{
	Vector3D<float> vertex;
	Vector2D<float> tex_coords;
	MyVertex() { }
	MyVertex(Vector3D<float> v,Vector2D<float> uv)
	{
		vertex = v;
		tex_coords = uv;
	}
};
	

void Application::Setup()
{
	//m_Device = m_Engine.CreateDevice(Graphics::EOpenGLES11);
	
	//Pxf::Resource::Image t_Image(new Pxf::Resource::Chunk(),"test.png");
	
	// Load some texture
	glEnable(GL_TEXTURE_2D);
	pTexture = m_Device->CreateTexture("test.png");
	
	pSprite = new Game::Sprite(m_Device,NULL,pTexture,100,50,10);
	pBuffer = m_Device->CreateVertexBuffer(Graphics::VB_LOCATION_GPU,Graphics::VB_USAGE_STATIC_DRAW);
	pBuffer->CreateNewBuffer(4,sizeof(Vector3D<float>) + sizeof(Vector2D<float>));
	pBuffer->SetData(Graphics::VB_VERTEX_DATA,0,3);
	pBuffer->SetData(Graphics::VB_TEXCOORD_DATA,sizeof(Vector3D<float>),2);
	pBuffer->SetPrimitive(Graphics::VB_PRIMITIVE_TRIANGLE_STRIP);
	
	MyVertex _Data[4];
	_Data[0] = MyVertex(Vector3D<float>(-0.5f,-0.5f,0.5f),Vector2D<float>(0.0f,1.0f));
	_Data[1] = MyVertex(Vector3D<float>(0.5f,-0.5f,0.5f),Vector2D<float>(1.0f,1.0f));
	_Data[2] = MyVertex(Vector3D<float>(-0.5f,0.5f,0.5f),Vector2D<float>(0.0f,0.0f));
	_Data[3] = MyVertex(Vector3D<float>(0.5f,0.5f,0.5f),Vector2D<float>(1.0f,0.0f));
	
	//Vector3D<float>* _MappedData = (Vector3D<float>*)pBuffer->MapData(Graphics::VB_ACCESS_WRITE_ONLY);
	
	pBuffer->UpdateData(_Data,sizeof(_Data),0);
	
	//pSprite->Reset();
	//m_Device->BindTexture(pTexture);
	
	// Lets create some quads, but render them in "reverse" order via SetDepth(...).
	/*Graphics::QuadBatch* pQBatch = m_Device->CreateQuadBatch(256);
	pQBatch->Reset();
	pQBatch->SetTextureSubset(0.0f, 0.0f, 32.0f / pTexture->GetWidth(), 32.0f / pTexture->GetHeight());
	pQBatch->SetDepth(0.5f);
	pQBatch->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
	pQBatch->AddCentered(200, 200, 50, 50);
	pQBatch->SetDepth(0.1f);
	pQBatch->SetColor(0.0f, 1.0f, 0.0f, 1.0f);
	pQBatch->AddCentered(225, 225, 50, 50);
	*/
}

bool Application::Init()
{
	m_Engine		= new Engine();	
	m_IsRunning		= true;
	
	if(!m_Engine)
	{
		m_IsRunning = false;
	}
	
	return m_IsRunning;
}

bool Application::IsRunning()
{
	return m_IsRunning;
}

void Application::Shutdown()
{
	m_IsRunning = false;
	printf("Application: Shutting down\n");
}
