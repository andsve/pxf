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

#define ERROR(str) printf("AppController ERROR: " #str "\n");

static Application* _Application = NULL;

@implementation AppController

-(void) update
{
	// UPDATE
	if(!_Application->Update())
		ERROR("unable to update applcation") 
	
	// RENDER
	if(!_Application->Render())
		ERROR("unable to render application")
	
	// SWAP BUFFER
}

-(void) applicationDidFinishLaunching:(UIApplication*)application
{
	_Application = new Application("pxf iPhone test");
	
	CGRect _Rect	= [[UIScreen mainScreen] bounds];
	m_Window		= [[UIWindow alloc] initWithFrame:_Rect];
	
	[m_Window makeKeyAndVisible]; 
}

-(void) dealloc
{
	[m_Window release];
	[super dealloc];
}
@end
