#import <Pxf/Graphics/EAGLView.h>
#import <OpenGLES/EAGLDrawable.h>
#import <QuartzCore/QuartzCore.h>

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
	
	if(!(m_FrameBuffer = (Pxf::Graphics::VideoBufferGL*) m_Device->CreateFrameBuffer()))
	{
		printf("Unable to create Frame Buffer\n");
		_RetVal = false;
	}
	else
	{
		//((Pxf::Graphics::DeviceGLES11*)m_Device)->BindVideoBuffer(m_FrameBuffer,GL_FRAMEBUFFER_OES);
		printf("Frame buffer OK\n");
	}


	if(!(m_RenderBuffer = (Pxf::Graphics::VideoBufferGL*) m_Device->CreateVideoBuffer(m_BackingWidth,m_BackingHeight,GL_RGBA8_OES)))
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
		if(!(m_DepthBuffer = (Pxf::Graphics::VideoBufferGL*) m_Device->CreateVideoBuffer(m_BackingWidth,m_BackingHeight,GL_DEPTH_COMPONENT16_OES)))
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

- (void) SwapBuffers
{
//	m_Device->SwapBuffers();

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