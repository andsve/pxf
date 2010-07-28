#ifndef __PXF_GRAPHICS_EAGLVIEW11_H__
#define __PXF_GRAPHICS_EAGLVIEW11_H__

#import <Pxf/Graphics/EAGLView.h>

@interface EAGLView11 : EAGLView
{
@private
    
}

- (bool) InitDevice;
- (id) init;
- (id) initWithRect: (CGRect) _Frame;
- (id) initWithRect: (CGRect) _Frame bufferFormat: (GLuint) _CBFormat depthFormat: (GLuint) _DBFormat preserveBackbuffer: (BOOL) _Retained;
- (bool) CreateSurface;

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)handleSingleTap:(NSDictionary *)touch;
- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event;

@end

#endif