/*
 *  Application.h
 *  project
 *
 *  Created by jhonny on 6/2/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _APPLICATION_H_
#define _APPLICATION_H_

//#define USE_DEPTH_BUFFER 1
//#define APP_FPS 1.0 // timer frequency

#include <Pxf/Util/MemoryManager/mmgr.h>

#include <Pxf/Pxf.h>
#include <Pxf/Engine.h>
#include <Pxf/Graphics/Device.h>
#include <Pxf/Scene/SceneManager.h>
#include <Pxf/Game/Sprite.h>
#include <Pxf/Graphics/Texture.h>
#include <Pxf/Graphics/QuadBatch.h>
#include <Pxf/Resource/Image.h>
#include <Pxf/Resource/Chunk.h>
#include <Box2D/Box2D.h>
#include <Pxf/Game/Box2D/Box2DPhysicsWorld.h>

#import <Pxf/Extra/iPhoneInput/iPhoneInput.h>
		

struct fps_helper
{
	float fps;
	int ticks;
	float elapsed_time;
};

class Application
{
public:
	Application(const char* _Title);
	~Application();
	
	bool Init();
	bool Destroy();
	bool Render();
	bool Update();
	bool IsRunning();
	
	void Shutdown();
	
	void SetDevice(Pxf::Graphics::Device* _pDevice);
	void Setup();
	
private:
	void _UpdateFPS();
	
	Pxf::Engine*				m_Engine;
	Pxf::Graphics::Device*		m_Device;
	bool						m_IsRunning;
	
	const char*					m_Title;
	fps_helper					m_FPS;
	
	// DEBUG OBJECTS
	Pxf::Game::Sprite*				pSprite1;
	Pxf::Game::Sprite*				pSprite2;
	Pxf::Graphics::VertexBuffer*	pBuffer;
	Pxf::Graphics::Texture*			pTexture;
	Pxf::Graphics::QuadBatch*		pQBatch;
	
	// BOX2D
	Pxf::Game::Box2DPhysicsWorld*	m_World;
	/*
	b2World		m_b2World;
	float		m_b2TimeStep;
	int32		m_b2VelIterations;
	int32		m_b2PosIterations; */
	
	// Input
	InputHandler* m_InputHandler;
};

#endif