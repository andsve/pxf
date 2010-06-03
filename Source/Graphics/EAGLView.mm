#import <Pxf/Graphics/EAGLView.h>
#import <OpenGLES/EAGLDrawable.h>
#import <QuartzCore/QuartzCore.h>

@implementation EAGLView
@synthesize m_FrameBuffer,m_DepthBuffer,m_RenderBuffer,m_Context; // LOL OBJECTIVE C

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
		printf("Frame buffer OK\n");


	if(!m_Device->CreateVideoBuffer(m_RenderBuffer,m_BackingWidth,m_BackingHeight,GL_RGBA8_OES))
	{
		printf("Unable to create Frame Buffer\n");	
		_RetVal = false;
	}

	if(m_UseDepthBuffer)
	{
		if(!m_Device->CreateVideoBuffer(m_DepthBuffer,m_BackingWidth,m_BackingHeight,GL_DEPTH_COMPONENT16_OES))
		{
			printf("Unable to create Frame Buffer\n");	
			_RetVal = false;
		}
	}
	
	return _RetVal;	
}

- (void) SwapBuffers
{
//	m_Device->SwapBuffers();

	NSAssert(m_Context,@"Invalid Context");
	NSAssert(m_RenderBuffer.m_Handle,@"Invalid RenderBuffer");
	
	// fetch current context to make sure we are working on the correct one 
	EAGLContext* _OldContext = [EAGLContext currentContext];
	
	if(_OldContext != m_Context)
		[EAGLContext setCurrentContext: m_Context];
	
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, m_RenderBuffer.m_Handle);
	if(![m_Context presentRenderbuffer:GL_RENDERBUFFER_OES])
		printf("Swap buffers failed/n");

	// activate old context
	if(_OldContext != m_Context)
		[EAGLContext setCurrentContext: _OldContext];
}

@end