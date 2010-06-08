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
	
	
	const GLfloat squareVertices[] = {
        -0.5f, -0.5f,
        0.5f,  -0.5f,
        -0.5f,  0.5f,
        0.5f,   0.5f,
    };
    const GLubyte squareColors[] = {
        255, 255,   0, 255,
        0,   255, 255, 255,
        0,     0,   0,   0,
        255,   0, 255, 255,
    };
	
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrthof(-1.0f, 1.0f, -1.5f, 1.5f, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glRotatef(3.0f, 0.0f, 0.0f, 1.0f);
    
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glVertexPointer(2, GL_FLOAT, 0, squareVertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, squareColors);
    glEnableClientState(GL_COLOR_ARRAY);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	return _RetVal;
}

bool Application::Init()
{
	bool _RetVal	= true;
	
	m_Engine		= new Engine();
	m_SceneManager	= new Scene::SceneManager();
	
	m_IsRunning		= true;
	
	if(!(m_Engine && m_SceneManager))
	   return false;
	
	return _RetVal;
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
