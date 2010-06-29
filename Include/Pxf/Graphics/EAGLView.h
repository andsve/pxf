#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#include <Pxf/Graphics/Device.h>
#include <Pxf/Graphics/OpenGL/DeviceGLES11.h>
#include <Pxf/Graphics/OpenGL/VideoBufferGL.h>

@interface EAGLView : UIView
{
	@public
	Pxf::Graphics::Device*	m_Device;
}

- (void) PrepareDrawing;

@end

