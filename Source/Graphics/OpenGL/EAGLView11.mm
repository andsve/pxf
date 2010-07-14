#import <Pxf/Graphics/OpenGL/EAGLView11.h>
#import <Pxf/Graphics/OpenGL/DeviceGLES11.h>
#import <QuartzCore/QuartzCore.h>

#include <Pxf/Base/Debug.h>

#define LOCAL_MSG "EAGLView11"

@implementation EAGLView11

+ (Class) layerClass
{
	return [CAEAGLLayer class];
}

///////////////////////////////////
// Touch stuff
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {

    UITouch *aTouch = [touches anyObject];

    if (aTouch.tapCount == 2) {

        [NSObject cancelPreviousPerformRequestsWithTarget:self];

    }

}

 

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {

    UITouch *aTouch = [touches anyObject];

    CGPoint loc = [aTouch locationInView:self];

    CGPoint prevloc = [aTouch previousLocationInView:self];

    //printf("Drag: %f %f\n", loc.x - prevloc.x, loc.y - prevloc.y);
    ((Pxf::Graphics::DeviceGLES11 *)m_Device)->InputSetDrag(loc.x, loc.y, prevloc.x, prevloc.y);

}

 

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {

    UITouch *theTouch = [touches anyObject];

    if (theTouch.tapCount == 1) {

        NSDictionary *touchLoc = [NSDictionary dictionaryWithObject:
            [NSValue valueWithCGPoint:[theTouch locationInView:self]] forKey:@"location"];

        [self performSelector:@selector(handleSingleTap:) withObject:touchLoc afterDelay:0.1];

    } else if (theTouch.tapCount == 2) {

        // Double-tap: increase image size by 10%"
        
        //printf("Heyo, double-tap!\n");
        CGPoint tapPoint = [theTouch locationInView:self];
        ((Pxf::Graphics::DeviceGLES11 *)m_Device)->InputSetDoubleTap(tapPoint.x, tapPoint.y);
    }

} 

- (void)handleSingleTap:(NSDictionary *)touch {

    // Single-tap: decrease image size by 10%"
    CGPoint tapPoint = [[touch valueForKey:@"location"] CGPointValue];
    //printf("Heyo tap: %f %f\n", tapPoint.x, tapPoint.y);
    ((Pxf::Graphics::DeviceGLES11 *)m_Device)->InputSetTap(tapPoint.x, tapPoint.y);

}

 

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {

    /* no state to clean up, so null implementation */

}
////////////////////////////////

- (id) init
{
	if(self = [super init]) 
	{
		if([self InitDevice])
			Pxf::Message(LOCAL_MSG,"Device is ready to use");
	}
		
	return self;
}

- (bool) InitDevice
{
	m_Device = new Pxf::Graphics::DeviceGLES11();
	
	((Pxf::Graphics::DeviceGLES11*)m_Device)->SetUIView(self);
	
	return m_Device->Ready();
}

-(void) DestroySurface
{
	EAGLContext* _OldContext	= [EAGLContext currentContext];
	EAGLContext* _Context		= ((Pxf::Graphics::DeviceGLES11*) m_Device)->GetEAGLContext();
	
	if(_OldContext != _Context)
		[EAGLContext setCurrentContext:_Context];
	
	if(_OldContext != _Context)
		[EAGLContext setCurrentContext:_OldContext];
		
}

-(bool) CreateSurface
{
	EAGLContext*	_Context	= ((Pxf::Graphics::DeviceGLES11*) m_Device)->GetEAGLContext();
	CAEAGLLayer*	_EaglLayer	= (CAEAGLLayer*)[self layer];
	CGSize			_Size		= [_EaglLayer bounds].size;
	
	((Pxf::Graphics::DeviceGLES11*) m_Device)->SetBackingWidth(roundf(_Size.width));
	((Pxf::Graphics::DeviceGLES11*) m_Device)->SetBackingHeight(roundf(_Size.height));
	
	if(!((Pxf::Graphics::DeviceGLES11*) m_Device)->InitBuffers(_Context, _EaglLayer))
	{
		[self release];
		Pxf::Message(LOCAL_MSG,"Unable to initialize buffers");
		return false;
	}
	else
		Pxf::Message(LOCAL_MSG,"Render/frame buffers: OK");
	
	// Setup input handling
	((Pxf::Graphics::DeviceGLES11*) m_Device)->InitInput();
	
	return true;
}


/*
-(bool) CreateSurface
{
	EAGLContext*	_Context	= ((Pxf::Graphics::DeviceGLES11*) m_Device)->GetEAGLContext();
	CAEAGLLayer*	_EaglLayer	= (CAEAGLLayer*)[self layer];
	CGSize			_Size		= [_EaglLayer bounds].size;
	
	((Pxf::Graphics::DeviceGLES11*) m_Device)->SetBackingWidth(roundf(_Size.width));
	((Pxf::Graphics::DeviceGLES11*) m_Device)->SetBackingHeight(roundf(_Size.height));
	
	if(!((Pxf::Graphics::DeviceGLES11*) m_Device)->InitBuffers())
	{
		[self release];
		Pxf::Message(LOCAL_MSG,"Unable to initialize buffers");
		return false;
	}
	else
		Pxf::Message(LOCAL_MSG,"Init buffers OK");
	
	if(!([_Context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:_EaglLayer]))
	{
		Pxf::Message(LOCAL_MSG,"Unable to allocate render buffer storage");
		return false;
	}
	else
		Pxf::Message(LOCAL_MSG,"Allocate render buffer storage OK");
	
	
	Pxf::Graphics::VideoBufferGL* _FrameBuffer = ((Pxf::Graphics::DeviceGLES11*) m_Device)->GetFrameBuffer();
	Pxf::Graphics::VideoBufferGL* _RenderBuffer = ((Pxf::Graphics::DeviceGLES11*) m_Device)->GetRenderBuffer();
	Pxf::Graphics::VideoBufferGL* _DepthBuffer = ((Pxf::Graphics::DeviceGLES11*) m_Device)->GetDepthBuffer();
	
	((Pxf::Graphics::DeviceGLES11*) m_Device)->UnBindVideoBufferType(GL_RENDERBUFFER_OES);
	((Pxf::Graphics::DeviceGLES11*) m_Device)->UnBindVideoBufferType(GL_FRAMEBUFFER_OES);
	
	((Pxf::Graphics::DeviceGLES11*) m_Device)->BindVideoBuffer(_FrameBuffer);
	((Pxf::Graphics::DeviceGLES11*) m_Device)->BindVideoBuffer(_RenderBuffer);
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, _RenderBuffer->m_Handle);
	
	if(((Pxf::Graphics::DeviceGLES11*) m_Device)->GetUseDepthBuffer())
	{
	    ((Pxf::Graphics::DeviceGLES11*) m_Device)->BindVideoBuffer(_DepthBuffer);
		glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, _DepthBuffer->m_Handle);
	}
	
	 // check for completeness
	 GLenum _Status = glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES);
	 char _MSG[256];
	 if(_Status != GL_FRAMEBUFFER_COMPLETE_OES)
	 {
		 
		 switch(_Status)
		 {
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_OES:
				 sprintf(_MSG, "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_OES");
				 break;
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_OES:
				 sprintf(_MSG, "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_OES");
				 break;
			default:
				 sprintf(_MSG, "Unknown");
				break;
		}
		 
		Pxf::Message(LOCAL_MSG,"Failed to create complete framebuffer object. Reason: %s", _MSG);
	 
		return false;
	 }
	 else
		Pxf::Message(LOCAL_MSG,"Frame buffer Complete");
	
	return true;
}*/

- (void) dealloc
{
	[self DestroySurface];
	[super dealloc];
}

- (void) layoutSubViews
{
	EAGLContext* _Context = ((Pxf::Graphics::DeviceGLES11*) m_Device)->GetEAGLContext();
	[EAGLContext setCurrentContext:_Context];
	[self DestroySurface];
	[self CreateSurface];
}

- (id) disableStatusbar:(BOOL) _toggle
{
	if (_toggle)
	{
		[[UIApplication sharedApplication] setStatusBarHidden:YES animated:NO];
	} else {
		[[UIApplication sharedApplication] setStatusBarHidden:NO animated:NO];
	}
	
	return self;
}

- (id) initWithRect: (CGRect) _Frame
{
	return [self initWithRect: _Frame bufferFormat: GL_RGBA8_OES depthFormat: GL_DEPTH_ATTACHMENT_OES preserveBackbuffer: NO];
}

- (id) initWithRect: (CGRect) _Frame bufferFormat: (GLuint) _CBFormat depthFormat: (GLuint) _DBFormat preserveBackbuffer: (BOOL) _Retained
{
	if(self = [super initWithFrame:_Frame])
	{	
		EAGLContext*	_Context	= [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
		
		if(!_DBFormat)
			((Pxf::Graphics::DeviceGLES11*) m_Device)->SetUseDepthBuffer(false);
		else
		    ((Pxf::Graphics::DeviceGLES11*) m_Device)->SetUseDepthBuffer(true);
		   
		CAEAGLLayer* _EaglLayer = (CAEAGLLayer*)[self layer];
		_EaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys: 
										 [NSNumber numberWithBool:_Retained], 
										 kEAGLDrawablePropertyRetainedBacking,
										 (_CBFormat == GL_RGB565_OES) ? kEAGLColorFormatRGB565 : kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
		
		((Pxf::Graphics::DeviceGLES11*) m_Device)->SetEAGLContext(_Context);
		
		if(_Context == nil || ![EAGLContext setCurrentContext:_Context])
		{
			Pxf::Message(LOCAL_MSG,"Unable to initialize with empty context");
			[self release]; 
			return nil;
		}
		
		if(![self CreateSurface])
		{
			[self release];
			Pxf::Message(LOCAL_MSG,"Unable to create surface");
			return nil;
		}
		
	}
					 
	return self;
}

@end