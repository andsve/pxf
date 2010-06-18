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
	
	//Pxf::Scene::SceneManager* SceneMgr() { return m_SceneManager; }
	
private:
	void _UpdateFPS();
	
	//Pxf::Scene::SceneManager*	m_SceneManager;
	Pxf::Engine*				m_Engine;
	Pxf::Graphics::Device*		m_Device;
	bool						m_IsRunning;
	
	const char*					m_Title;
	fps_helper					m_FPS;
	
	// DEBUG OBJECTS
	Pxf::Game::Sprite*				pSprite;	
	Pxf::Graphics::VertexBuffer*	pBuffer;
	Pxf::Graphics::Texture*			pTexture;
	Pxf::Graphics::QuadBatch*		pQBatch;
};

#endif