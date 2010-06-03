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
	

- (id) initWithRect: (CGRect) _Frame
{
	return [self initWithRect: _Frame bufferFormat: GL_RGB565_OES depthFormat: 0 preserveBackbuffer: NO];
}

- (id) initWithRect: (CGRect) _Frame bufferFormat: (GLuint) _CBFormat depthFormat: (GLuint) _DBFormat preserveBackbuffer: (BOOL) _Retained
{
	if(self = [super initWithFrame:_Frame])
	{		
		if(!_DBFormat)
			m_UseDepthBuffer = false;
		
		[self InitBuffers];
		   
		CAEAGLLayer* _EaglLayer = (CAEAGLLayer*)[self layer];
		
		m_Context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
		
		if(m_Context == nil)
		{
			[self release]; 
			return nil;
		}
	}
					 
	return self;
}

@end