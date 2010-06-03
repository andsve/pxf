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

#define APP_FPS 1.0 // timer frequency

#include <Pxf/Util/MemoryManager/mmgr.h>

#include <Pxf/Pxf.h>
#include <Pxf/Engine.h>
#include <Pxf/Scene/SceneManager.h>

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
	
private:
	Pxf::Scene::SceneManager* m_SceneManager;
	Pxf::Engine* m_Engine;
	
	const char* m_Title;
	fps_helper m_FPS;
};

#endif