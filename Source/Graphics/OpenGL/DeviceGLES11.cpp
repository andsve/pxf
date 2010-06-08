#include <Pxf/Graphics/OpenGL/DeviceGLES11.h>
#include <Pxf/Base/Debug.h>
#include <OpenGLES/ES1/glext.h>
#include <stdio.h>

using namespace Pxf;
using namespace Graphics;

DeviceGLES11::DeviceGLES11()
{
	
}

DeviceGLES11::~DeviceGLES11()
{
}

Window* DeviceGLES11::OpenWindow(WindowSpecifications* _pWindowSpecs)
{
	return 0;
}
void DeviceGLES11::CloseWindow()
{
}

// Graphics
void DeviceGLES11::SetViewport(int _x, int _y, int _w, int _h)
{
	glViewport(_x,_y,_w,_h);	
}

void DeviceGLES11::SetProjection(Math::Mat4 *_matrix)
{

}

void DeviceGLES11::Translate(Math::Vec3f _translate)
{
}

// Texture
Texture* DeviceGLES11::CreateEmptyTexture(int _Width,int _Height,TextureFormatStorage _Format)
{
	return 0;
}
Texture* DeviceGLES11::CreateTexture(const char* _filepath)
{
	return 0;
}
Texture* DeviceGLES11::CreateTextureFromData(const unsigned char* _datachunk, int _width, int _height, int _channels)
{
	return 0;
}
void DeviceGLES11::BindTexture(Texture* _texture)
{
}
void DeviceGLES11::BindTexture(Texture* _texture, unsigned int _texture_unit)
{
}

// PrimitiveBatch
QuadBatch* DeviceGLES11::CreateQuadBatch(int _maxSize)
{
	return 0;
}

VertexBuffer* DeviceGLES11::CreateVertexBuffer(VertexBufferLocation _VertexBufferLocation, VertexBufferUsageFlag _VertexBufferUsageFlag)
{
	return 0;
}
void DeviceGLES11::DestroyVertexBuffer(VertexBuffer* _pVertexBuffer)
{
}
void DeviceGLES11::DrawBuffer(VertexBuffer* _pVertexBuffer)
{
}

void DeviceGLES11::BindRenderTarget(RenderTarget* _RenderTarget)
{
}
void DeviceGLES11::ReleaseRenderTarget(RenderTarget* _RenderTarget)
{
}
RenderTarget* DeviceGLES11::CreateRenderTarget(int _Width,int _Height,RTFormat _ColorFormat,RTFormat _DepthFormat)
{
	return 0;
}

VideoBuffer* DeviceGLES11::CreateVideoBuffer(int _Width, int _Height, int _Format)
{
	VideoBufferGL* _NewVB = new VideoBufferGL();
	_NewVB->m_Width = _Width;
	_NewVB->m_Height = _Height;
	_NewVB->m_Target = GL_RENDERBUFFER_OES;
	
	glGenRenderbuffersOES(1, &_NewVB->m_Handle);
	
	BindVideoBuffer(_NewVB);
	glRenderbufferStorageOES(GL_RENDERBUFFER_OES,_Format,_Width,_Height);
	
	
	return _NewVB;
}

void DeviceGLES11::DeleteVideoBuffer(VideoBuffer* _VideoBuffer)
{
	int _Target = ((VideoBufferGL*) _VideoBuffer)->m_Target;
	
	switch(_Target)
	{
		case GL_FRAMEBUFFER_OES:
			glDeleteFramebuffersOES(1,&((VideoBufferGL*) _VideoBuffer)->m_Handle);
			break;
		case GL_RENDERBUFFER_OES:
			glDeleteRenderbuffersOES(1,&((VideoBufferGL*) _VideoBuffer)->m_Handle);
			break;
		default:
			break;
	
		((VideoBufferGL*) _VideoBuffer)->m_Handle = 0;
	}
}
	

bool DeviceGLES11::BindVideoBuffer(VideoBuffer* _VideoBuffer)
{
	int _Target = ((VideoBufferGL*) _VideoBuffer)->m_Target;
	
	switch(_Target)
	{
		case GL_FRAMEBUFFER_OES:
			glBindFramebufferOES(GL_FRAMEBUFFER_OES, ((VideoBufferGL*) _VideoBuffer)->m_Handle);
			break;
		case GL_RENDERBUFFER_OES:
			glBindRenderbufferOES(GL_RENDERBUFFER_OES, ((VideoBufferGL*) _VideoBuffer)->m_Handle);
			break;
		default:
			break;
	}
	
	// check status?
	return true;
}
	

VideoBuffer* DeviceGLES11::CreateFrameBuffer()
{
	VideoBufferGL* _NewVB = new VideoBufferGL();
	_NewVB->m_Target = GL_FRAMEBUFFER_OES;
	glGenFramebuffersOES(1,&_NewVB->m_Handle);	
	return _NewVB;
}

void DeviceGLES11::SwapBuffers()
{
	//Pxf::Message("Device","SwapBuffers");
}