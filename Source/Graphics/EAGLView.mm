#import <Pxf/Graphics/EAGLView.h>
#import <OpenGLES/EAGLDrawable.h>
#import <QuartzCore/QuartzCore.h>

using namespace Pxf;
using namespace Graphics;

@implementation EAGLView

// TODO: remove..
- (void) PrepareDrawing
{
	m_Device->SetViewport(0,0,((Pxf::Graphics::DeviceGLES11*)m_Device)->GetBackingWidth(),
						  ((Pxf::Graphics::DeviceGLES11*)m_Device)->GetBackingHeight());
}

@end