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
#include <Pxf/Graphics/OpenGL/DeviceGLES11.h>
#include <Pxf/Extra/LuaGame/LuaGame.h>

//#include <Box2D/lol.h>

using namespace Pxf;
using namespace Math;
using namespace Graphics;
using namespace Extra;

/*
typedef signed char	int8;
typedef signed short int16;
typedef signed int int32;
typedef unsigned int uint32;
*/
 
float32 timeStep = 1.0f / 60.0f;
int32 velocityIterations = 6;
int32 positionIterations = 2;
b2Vec2	gravity(0.0f, -10.0f);
bool	doSleep = true;
b2World world(gravity, doSleep);
b2Body* body1;
b2Body* body2;

// LuaGame instance
LuaGame* luagame;

Application::Application(const char* _Title)
	: m_Title(_Title),
	  m_Engine(0),
	  m_IsRunning(false)
{
	// Workaround to get correct resources dir under osx
    NSString* readPath = [[NSBundle mainBundle] resourcePath];
	char buffer[2048];
	[readPath getCString:buffer maxLength:2048 encoding:NSUTF8StringEncoding];
	chdir(buffer);
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
	
	world.Step(1.0f / 60.0f, velocityIterations, positionIterations);
    world.ClearForces();
	
	b2Vec2 pos = body1->GetPosition();
	pSprite1->SetPosition(Vec3f(pos.x,pos.y,0.0f));
	
	pos = body2->GetPosition();
	pSprite2->SetPosition(Vec3f(pos.x,pos.y,0.0f));
	
	_UpdateFPS();
	
	// Update LuaGame
	luagame->Update(0.1);

	
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
	
	//m_Device->BindTexture(pTexture);
	
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	glOrthof(0.0f, 10.0f, 0.f, 10.0f, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
	pSprite1->Draw();
	pSprite2->Draw();
	
	// Draw loading if preloading resources
	luagame->Render();
	
	return _RetVal;
}

void Application::SetDevice(Pxf::Graphics::Device* _pDevice)
{
	m_Device = _pDevice;
	
	Setup();
}

/*
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
}; */
	

void Application::Setup()
{		
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, -10.0f);
	b2Body* groundBody = world.CreateBody(&groundBodyDef);
	b2PolygonShape groundBox;
	groundBox.SetAsBox(50.0f, 10.0f);
	groundBody->CreateFixture(&groundBox,1);
	
	// body 1:
	b2BodyDef		bodyDef1;
	b2FixtureDef	fixtureDef1;
	b2PolygonShape	dynamicBox1;
	
	bodyDef1.type = b2_dynamicBody;
	bodyDef1.position.Set(5.0f, 4.0f);
	body1 = world.CreateBody(&bodyDef1);

	dynamicBox1.SetAsBox(0.5f, 0.5f);
	
	fixtureDef1.shape = &dynamicBox1;
	fixtureDef1.density = 1.0f;
	fixtureDef1.friction = 0.3f;
	
	body1->CreateFixture(&fixtureDef1);
	
	// body 2:
	b2BodyDef		bodyDef2;
	b2FixtureDef	fixtureDef2;
	b2PolygonShape	dynamicBox2;
	
	bodyDef2.type = b2_dynamicBody;
	bodyDef2.position.Set(5.2f, 10.0f);
	body2 = world.CreateBody(&bodyDef2);
	
	dynamicBox2.SetAsBox(0.5f, 0.5f);
	
	fixtureDef2.shape = &dynamicBox2;
	fixtureDef2.density = 1.0f;
	fixtureDef2.friction = 0.3f;
	
	body2->CreateFixture(&fixtureDef2);
	
	
	//m_Device = m_Engine.CreateDevice(Graphics::EOpenGLES11);
	
	//Pxf::Resource::Image t_Image(new Pxf::Resource::Chunk(),"test.png");
	
	// Load some texture
	glEnable(GL_TEXTURE_2D);
	pTexture = m_Device->CreateTexture("sprite_test.jpg");
	
	pSprite1 = new Game::Sprite(m_Device,						// Device Context
							   Vec2f(0.0f,0.0f),				// Object Position
							   "MySprite",						// Object Name (providing a NULL pointer 
																// in this field will generate a new name)
							   pTexture,						// Sprite Texture
							   64,								// Sprite Cell Width
							   64,								// Sprite Cell Height
							   10,								// Sprite Draw Frequency
							   -1);								// Sprite Depth Sort, -1 = NO SORT
	
	pSprite2 = new Game::Sprite(m_Device,						// Device Context
								Vec2f(0.0f,0.0f),				// Object Position
								"MySprite2",						// Object Name (providing a NULL pointer 
								// in this field will generate a new name)
								pTexture,						// Sprite Texture
								64,								// Sprite Cell Width
								64,								// Sprite Cell Height
								10,								// Sprite Draw Frequency
								-1);								// Sprite Depth Sort, -1 = NO SORT
	
	/*
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
	 */
	
	//Vector3D<float>* _MappedData = (Vector3D<float>*)pBuffer->MapData(Graphics::VB_ACCESS_WRITE_ONLY);
	
	//pBuffer->UpdateData(_Data,sizeof(_Data),0);
	
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
	
	// Load LuaGame
	luagame->Load();
}

bool Application::Init()
{
	m_Engine		= new Engine();	
	m_IsRunning		= true;
	
	if(!m_Engine)
	{
		m_IsRunning = false;
	}
	
	// Init LuaGame
	luagame = new LuaGame("test.lua", m_Device);
	
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
