#import <Pxf/Graphics/EAGLView.h>

@interface EAGLView11 : EAGLView
{
@private

}

- (bool) InitDevice;
- (id) init;
- (id) initWithRect: (CGRect) _Frame;
- (id) initWithRect: (CGRect) _Frame bufferFormat: (GLuint) _CBFormat depthFormat: (GLuint) _DBFormat preserveBackbuffer: (BOOL) _Retained;

@end