#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#include <Pxf/Graphics/Device.h>
#include <Pxf/Graphics/OpenGL/VideoBufferGL.h>

@interface EAGLView : UIView
{
@protected
	/*GLuint					m_RenderBuffer;
	GLuint					m_FrameBuffer;
	GLuint					m_DepthBuffer; */
	bool	m_UseDepthBuffer;
	GLint	m_BackingWidth;
	GLint	m_BackingHeight;
	
	Pxf::Graphics::VideoBufferGL	m_RenderBuffer;
	Pxf::Graphics::VideoBufferGL*	m_FrameBuffer;
	Pxf::Graphics::VideoBufferGL	m_DepthBuffer;
	
	EAGLContext*			m_Context;
	Pxf::Graphics::Device*	m_Device;
}

@property (assign)	 bool m_UseDepthBuffer;
@property (readonly) Pxf::Graphics::VideoBufferGL m_RenderBuffer;
@property (readonly) Pxf::Graphics::VideoBufferGL* m_FrameBuffer;
@property (readonly) Pxf::Graphics::VideoBufferGL m_DepthBuffer;
@property (readonly) EAGLContext* m_Context;

- (void) SwapBuffers;
- (bool) InitBuffers;

@end

