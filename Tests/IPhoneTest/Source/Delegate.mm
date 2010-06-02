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

//static Application* _Application = null;

@implementation AppController

-(void) update
{
	// UPDATE
	
	// RENDER
	
	// SWAP BUFFER
}

-(void) applicationDidFinishLaunching:(UIApplication*)application
{
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
