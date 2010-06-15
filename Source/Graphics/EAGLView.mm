#import <Pxf/Graphics/EAGLView.h>
#import <OpenGLES/EAGLDrawable.h>
#import <QuartzCore/QuartzCore.h>

using namespace Pxf;
using namespace Graphics;

@implementation EAGLView
@synthesize m_FrameBuffer,m_DepthBuffer,m_RenderBuffer,m_Context,m_UseDepthBuffer; // LOL OBJECTIVE C

- (bool) InitBuffers
{	
	bool _RetVal = true;
	
	if(!m_Device)
	{
		printf("Trying to initialize buffers with an uninitalized device\n");
		_RetVal = false;
	}
	
	if(!(m_FrameBuffer = (VideoBufferGL*) ((DeviceGLES11*) m_Device)->CreateVideoBuffer(GL_FRAMEBUFFER_OES)))
	{
		printf("Unable to create Frame Buffer\n");
		_RetVal = false;
	}
	else
	{
		printf("Frame buffer OK\n");
	}


	if(!(m_RenderBuffer = (VideoBufferGL*) ((DeviceGLES11*) m_Device)->CreateVideoBuffer(GL_RGBA8_OES,m_BackingWidth,m_BackingHeight)))
	{
		printf("Unable to create Frame Buffer\n");	
		_RetVal = false;
	}
	else
	{		
		printf("Render buffer OK\n");
	}

	if(m_UseDepthBuffer)
	{
		if(!(m_DepthBuffer = (VideoBufferGL*) ((DeviceGLES11*) m_Device)->CreateVideoBuffer(GL_DEPTH_COMPONENT16_OES,m_BackingWidth,m_BackingHeight)))
		{
			printf("Unable to create Depth Buffer\n");	
			_RetVal = false;
		}
		else
		{
			// depth buffer ok, attach it to framebuffer
			printf("Depth buffer OK\n");
		}
	}
	 
	return _RetVal;	
}

- (void) PrepareDrawing
{
	m_Device->SetViewport(0,0,m_BackingWidth,m_BackingHeight);
}

- (void) SwapBuffers
{
	NSAssert(m_Context,@"Invalid Context");
	NSAssert(m_RenderBuffer->m_Handle,@"Invalid RenderBuffer");
	
	// fetch current context to make sure we are working on the correct one 
	EAGLContext* _OldContext = [EAGLContext currentContext];
	
	if(_OldContext != m_Context)
		[EAGLContext setCurrentContext: m_Context];
	
	((Pxf::Graphics::DeviceGLES11*)m_Device)->BindVideoBuffer(m_RenderBuffer);
	
	if(![m_Context presentRenderbuffer:GL_RENDERBUFFER_OES])
		printf("Swap buffers failed/n");

	// activate old context
	if(_OldContext != m_Context)
		[EAGLContext setCurrentContext: _OldContext];
}

@end