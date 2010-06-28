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

#include <Pxf/Game/Box2D/Box2DPhysicsWorld.h>
#include <Pxf/Game/PhysicsBodyDefs.h>

//#include <Box2D/lol.h>

#define LOCAL_MSG "Application"

using namespace Pxf;
using namespace Math;
using namespace Graphics;
using namespace Extra;

b2Body* _GroundBody; 
b2Body* _Box1Body; 

// LuaGame instance
LuaGame::Game* luagame;

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
	delete m_World;
	
	m_Engine = 0;
	m_World = 0;
}
	

bool Application::Update()
{
	bool _RetVal = true;
	// Call update on scene
	
	m_World->Simulate();
	m_World->ClearForces();
	
	/*
	world.Step(1.0f / 60.0f, velocityIterations, positionIterations);
    world.ClearForces();
	
	b2Vec2 pos = body1->GetPosition();
	pSprite1->SetPosition(Vec3f(pos.x,pos.y,0.0f));
	
	pos = body2->GetPosition();
	pSprite2->SetPosition(Vec3f(pos.x,pos.y,0.0f));
	 */
	
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
	
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	//glOrthof(0.0f, ((DeviceGLES11*) m_Device)->GetBackingWidth(), 0.f, ((DeviceGLES11*) m_Device)->GetBackingHeight(), -1.0f, 1.0f);
	glOrthof(0.0f, 10.0f, 0.f, 10.0f, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
	Vec3f _Pos(_Box1Body->GetPosition().x,_Box1Body->GetPosition().y,0.0f);
	//printf("%f,%f\n",_Pos.x,_Pos.y);
	m_Device->Translate(_Pos);
	pSprite1->Draw();
	m_Device->Translate(-_Pos);
	
	//pSprite2->Draw();
	
	// Draw loading if preloading resources
	luagame->Render();
	
	return _RetVal;
}

void Application::SetDevice(Pxf::Graphics::Device* _pDevice)
{
	m_Device = _pDevice;
	
	Setup();
}	

void Application::Setup()
{
	
	Vec2f _Gravity(0.05,-10.0f);
	m_World = new Game::Box2DPhysicsWorld(_Gravity,true,1 / 60.0f, 6,2);
	
	Game::body_parameters _GroundBodyParams;
	_GroundBodyParams.position.x = 0.0f;
	_GroundBodyParams.position.y = -10.0f;
	_GroundBodyParams.half_size.x = 50.0f;
	_GroundBodyParams.half_size.y = 10.0f;
	_GroundBodyParams.shape_type = b2Shape::e_polygon;
	_GroundBodyParams.po_type = Game::PO_BODY_STATIC;
	
	_GroundBody = m_World->CreateBodyFromParams(_GroundBodyParams);
	printf("%f,%f\n",_GroundBody->GetPosition().x,_GroundBody->GetPosition().y);
	
	Game::body_parameters _Box1;
	_Box1.position.x = 0.0f;
	_Box1.position.y = 50.0f;
	_Box1.half_size.x = 0.5f;
	_Box1.half_size.y = 0.5;
	_Box1.friction = 0.3f;
	_Box1.density = 1.0f;
	_Box1.shape_type = b2Shape::e_polygon;
	_Box1.po_type = Game::PO_BODY_DYNAMIC;
	
	_Box1Body = m_World->CreateBodyFromParams(_Box1);
	 
	// Load some texture
	pTexture = m_Device->CreateTexture("sprite_test.jpg");
	
	pSprite1 = new Game::Sprite(m_Device,						// Device Context
							   "MySprite",						// Object Name (providing a NULL pointer 
																// in this field will generate a new name)
							   pTexture,						// Sprite Texture
							   64,								// Sprite Cell Width
							   64,								// Sprite Cell Height
							   10,								// Sprite Draw Frequency
							   -1);								// Sprite Depth Sort, -1 = NO SORT
	/*
	pSprite2 = new Game::Sprite(m_Device,						// Device Context
								"MySprite2",						// Object Name (providing a NULL pointer 
								// in this field will generate a new name)
								pTexture,						// Sprite Texture
								64,								// Sprite Cell Width
								64,								// Sprite Cell Height
								10,								// Sprite Draw Frequency
								-1);								// Sprite Depth Sort, -1 = NO SORT
	 
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
	luagame = new LuaGame::Game("test.lua", m_Device);
	
	return m_IsRunning;
}

bool Application::IsRunning()
{
	return m_IsRunning;
}

void Application::Shutdown()
{
	m_IsRunning = false;
	Message(LOCAL_MSG,"Application: Shutting down\n");
}
