/*
 *  Application.cpp
 *  project
 *
 *  Created by jhonny on 6/2/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "../Include/Application.h"

using namespace Pxf;

Application::Application(const char* _Title)
{
	m_Title = _Title;
	m_Engine = 0;
	m_SceneManager = 0;
}

Application::~Application()
{
	m_Title = 0;
	delete m_Engine;
	delete m_SceneManager;
}
	

bool Application::Update()
{
	bool _RetVal = true;
	// Call update on scene
	
	return _RetVal;
}

bool Application::Render()
{
	bool _RetVal = true;
	// Call render on scene 
	
	return _RetVal;
}

bool Application::Init()
{
	bool _RetVal	= true;
	
	m_Engine		= new Engine();
	m_SceneManager	= new Scene::SceneManager();
	
	if(!(m_Engine && m_SceneManager))
	   return false;
	
	return _RetVal;
}