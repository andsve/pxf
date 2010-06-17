#include <Pxf/Graphics/OpenGL/DeviceGLES11.h>
#include <Pxf/Graphics/OpenGL/VertexBufferGLES11.h>
#include <Pxf/Graphics/OpenGL/TextureGLES.h>
#include <Pxf/Base/Debug.h>
#include <OpenGLES/ES1/gl.h>
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

void DeviceGLES11::_ConfigureTextureUnits()
{
	// In ES11, the second texture unit must be configured as to yield proper results..
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
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((GLfloat*)(_matrix->m));
	glMatrixMode(GL_MODELVIEW);	
}

void DeviceGLES11::Translate(Math::Vec3f _translate)
{
	glTranslatef(_translate.x,_translate.y,_translate.z);
}

// Texture
Texture* DeviceGLES11::CreateEmptyTexture(int _Width,int _Height,TextureFormatStorage _Format)
{
	TextureGLES* _Tex = new TextureGLES(this);
	_Tex->LoadData(NULL,_Width,_Height,_Format);
	return _Tex;
}
Texture* DeviceGLES11::CreateTexture(const char* _filepath)
{
	TextureGLES* _Tex = new TextureGLES(this);
	_Tex->Load(_filepath);
	
	return _Tex;
}
Texture* DeviceGLES11::CreateTextureFromData(const unsigned char* _datachunk, int _width, int _height, int _channels)
{
	TextureGLES* _Tex = new TextureGLES(this);
	_Tex->LoadData(_datachunk,_width,_height,_channels);
	
	return _Tex;
}
void DeviceGLES11::BindTexture(Texture* _texture)
{
	glBindTexture(GL_TEXTURE_2D,((TextureGLES*) _texture)->GetTextureID());
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
	return new VertexBufferGLES11(this, _VertexBufferLocation, _VertexBufferUsageFlag);
}
void DeviceGLES11::DestroyVertexBuffer(VertexBuffer* _pVertexBuffer)
{
	if(_pVertexBuffer)
		delete _pVertexBuffer;
}

static unsigned LookupPrimitiveType(VertexBufferPrimitiveType _PrimitiveType)
{
	switch(_PrimitiveType)
	{
		case VB_PRIMITIVE_POINTS:		return GL_POINTS;
		case VB_PRIMITIVE_LINES:		return GL_LINES;
		case VB_PRIMITIVE_LINE_LOOP:	return GL_LINE_LOOP;
		case VB_PRIMITIVE_LINE_STRIP:	return GL_LINE_STRIP;
		case VB_PRIMITIVE_TRIANGLES:	return GL_TRIANGLES;
		case VB_PRIMITIVE_TRIANGLE_STRIP:	return GL_TRIANGLE_STRIP;
		case VB_PRIMITIVE_TRIANGLE_FAN:	return GL_TRIANGLE_FAN;
			// quads does not exist on iphone so just use triangle fans instead
		case VB_PRIMITIVE_QUADS:		return GL_TRIANGLE_FAN;	
		case VB_PRIMITIVE_QUAD_STRIP:	return GL_TRIANGLE_STRIP;
	}
	PXFASSERT(0, "Unknown primitive type.");
	return 0;
}

void DeviceGLES11::DrawBuffer(VertexBuffer* _pVertexBuffer)
{
	_pVertexBuffer->_PreDraw();
	GLuint primitive = LookupPrimitiveType(_pVertexBuffer->GetPrimitive());
	glDrawArrays(primitive,0,_pVertexBuffer->GetVertexCount());
	_pVertexBuffer->_PostDraw();
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

VideoBuffer* DeviceGLES11::CreateVideoBuffer(int _Format, int _Width, int _Height)
{
	VideoBufferGL* _NewVB = new VideoBufferGL();
	
	switch(_Format)
	{
		case GL_FRAMEBUFFER_OES:
			_NewVB->m_Target = GL_FRAMEBUFFER_OES;
			glGenFramebuffersOES(1,&_NewVB->m_Handle);	
			break;
			
		// TODO: replace default case with special cases for each format OR add _target to signature?
		default:
			_NewVB->m_Width = _Width;
			_NewVB->m_Height = _Height;
			_NewVB->m_Target = GL_RENDERBUFFER_OES;
	
			glGenRenderbuffersOES(1, &_NewVB->m_Handle);
	
			BindVideoBuffer(_NewVB);
			glRenderbufferStorageOES(GL_RENDERBUFFER_OES,_Format,_Width,_Height);
			break;
	}
	
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

void DeviceGLES11::SwapBuffers()
{
	//Pxf::Message("Device","SwapBuffers");
}