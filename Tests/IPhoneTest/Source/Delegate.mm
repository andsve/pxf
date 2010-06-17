/*
 *  Delegate.mm
 *  project
 *
 *  Created by jhonny on 6/2/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#import "../Include/Delegate.h"
#import "../Include/Application.h"
#import <Pxf/Graphics/OpenGL/EAGLView11.h>
#include "../Include/ApplicationDefines.h"

#define ERROR(str) printf("AppController ERROR: " str "\n");
#define SUCCESS(str) printf("AppController SUCCESS: " str "\n");

static Application* _Application = NULL;

@implementation AppController

-(void) update
{
	if(_Application->IsRunning())
	{
		// move to Application->GameStep() or something?
		
		[m_GLView PrepareDrawing];
		
		// UPDATE
		if(!_Application->Update())
			ERROR("unable to update applcation") 
	
		// RENDER
		if(!_Application->Render())
			ERROR("unable to render application")
		
		// SWAP BUFFER
		[m_GLView SwapBuffers];
	}
}

-(void) applicationDidFinishLaunching:(UIApplication*)application
{
	_Application = new Application("pxf iPhone test");
	
	if(_Application->Init())
		SUCCESS("init")
	else
		ERROR("init failed")
	
	CGRect _Rect	= [[UIScreen mainScreen] bounds];
	m_Window		= [[UIWindow alloc] initWithFrame:_Rect];
	m_GLView		= [[[EAGLView11 alloc] init] initWithRect: _Rect];
	
	if(!m_GLView)
	{
		ERROR("unable to initialize GLView")
		_Application->Shutdown();
		return;
	}
	
	[m_Window addSubview:m_GLView];
	[m_Window makeKeyAndVisible]; 
	[NSTimer scheduledTimerWithTimeInterval:(1.0 / APP_FPS) target:self selector:@selector(update) userInfo:nil repeats:YES];
	
	_Application->SetDevice( m_GLView->m_Device );//(Pxf::Graphics::Device*)[m_GLView m_Device] );
}

-(void) dealloc
{
	[m_Window release];
	[super dealloc];
}
@end
