#import <Pxf/Graphics/OpenGL/EAGLView11.h>
#import <Pxf/Graphics/OpenGL/DeviceGLES11.h>
#import <QuartzCore/QuartzCore.h>

@implementation EAGLView11

+ (Class) layerClass
{
	return [CAEAGLLayer class];
}

- (id) init
{
	if(self = [super init]) 
	{
		if([self InitDevice])
			printf("Device is ready to use\n");
	}
		
	return self;
}

- (bool) InitDevice
{
	Pxf::Graphics::DeviceGLES11* _Tmp = new Pxf::Graphics::DeviceGLES11();
	m_Device = (Pxf::Graphics::Device*) _Tmp;
	return m_Device->Ready();
}

-(void) DestroySurface
{
	EAGLContext* _OldContext = [EAGLContext currentContext];
	
	if(_OldContext != m_Context)
		[EAGLContext setCurrentContext:m_Context];
	
	((Pxf::Graphics::DeviceGLES11*) m_Device)->DeleteVideoBuffer(m_RenderBuffer);
	
	if(m_UseDepthBuffer)
		((Pxf::Graphics::DeviceGLES11*) m_Device)->DeleteVideoBuffer(m_DepthBuffer);
	
	((Pxf::Graphics::DeviceGLES11*) m_Device)->DeleteVideoBuffer(m_FrameBuffer);
	
	if(_OldContext != m_Context)
		[EAGLContext setCurrentContext:_OldContext];
		
}

-(bool) CreateSurface
{
	CAEAGLLayer*	_EaglLayer = (CAEAGLLayer*)[self layer];
	CGSize			_Size = [_EaglLayer bounds].size;
	
	m_BackingWidth = roundf(_Size.width);
	m_BackingHeight = roundf(_Size.height);
	
	if(![self InitBuffers])
	{
		[self release];
		printf("Unable to initialize buffers\n");
		return false;
	}
	
	if(!([m_Context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:_EaglLayer]))
	{
		((Pxf::Graphics::DeviceGLES11*) m_Device)->DeleteVideoBuffer(m_RenderBuffer);
		printf("Unable to allocate render buffer storage\n");
		return false;
	}
	
	((Pxf::Graphics::DeviceGLES11*) m_Device)->BindVideoBuffer(m_FrameBuffer);
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES,GL_COLOR_ATTACHMENT0_OES,GL_RENDERBUFFER_OES,m_RenderBuffer->m_Handle);
	
	if(m_UseDepthBuffer)
		glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES,GL_DEPTH_ATTACHMENT_OES,GL_RENDERBUFFER_OES,m_DepthBuffer->m_Handle);
	
	 // check for completeness
	 GLenum _Status = glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES);
	 if(_Status != GL_FRAMEBUFFER_COMPLETE_OES)
	 {
		 printf("Failed to create complete framebuffer object. Reason: ");
	 
		 switch(_Status)
		 {
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_OES:
				 printf("Incomplete attachment\n");
				 break;
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_OES:
				 printf("Incomplete dimensions\n");
				 break;
			default:
				printf("Unknown\n");
				break;
		}
	 
		return false;
	 }
	 else
		 printf("Frame buffer Complete\n");
	
	return true;
}

- (void) dealloc
{
	[self DestroySurface];
	[m_Context release];
	m_Context = 0;
	
	[super dealloc];
}

- (void) layoutSubViews
{
	[EAGLContext setCurrentContext:m_Context];
	[self DestroySurface];
	[self CreateSurface];
	
}

- (id) initWithRect: (CGRect) _Frame
{
	return [self initWithRect: _Frame bufferFormat: GL_RGBA8_OES depthFormat: 0 preserveBackbuffer: NO];
}

- (id) initWithRect: (CGRect) _Frame bufferFormat: (GLuint) _CBFormat depthFormat: (GLuint) _DBFormat preserveBackbuffer: (BOOL) _Retained
{
	if(self = [super initWithFrame:_Frame])
	{		
		if(!_DBFormat)
			m_UseDepthBuffer = false;
		   
		CAEAGLLayer* _EaglLayer = (CAEAGLLayer*)[self layer];
		_EaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys: 
										 [NSNumber numberWithBool:_Retained], kEAGLDrawablePropertyRetainedBacking, (_CBFormat == GL_RGB565_OES) ? kEAGLColorFormatRGB565 : kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
		
		m_Context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
		
		if(m_Context == nil || ![EAGLContext setCurrentContext:m_Context])
		{
			printf("Unable to initialize with empty context\n");
			[self release]; 
			return nil;
		}
		
		if(![self CreateSurface])
		{
			[self release];
			printf("Unable to create surface\n");
			return nil;
		}
	}
					 
	return self;
}

@end