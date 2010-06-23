#include <Pxf/Pxf.h>
#include <Pxf/Util/String.h>
#include <Pxf/Graphics/OpenGL/TextureGLES.h>
#include <Pxf/Base/Debug.h>

#include <OpenGLES/ES1/gl.h>
#include <SOIL.h>
#define LOCAL_MSG "TextureGLES"

using namespace Pxf;
using namespace Pxf::Graphics;
using Util::String;

TextureGLES::TextureGLES(Device* _pDevice)
: Texture(_pDevice)
{
	m_TextureID = 0;
}

TextureGLES::~TextureGLES()
{
	Unload();
}

Math::Vec4f TextureGLES::CreateTextureSubset(float _x1, float _y1, float _x2, float _y2)
{
	
	float xdelta, ydelta;
	xdelta = 1.0f / (float)m_Width;
	ydelta = 1.0f / (float)m_Height;
	
	Math::Vec4f coords;
	/*coords.x = _x1 * xdelta;
	 coords.y = 1.0f - _y1 * ydelta;
	 coords.z = coords.x + _x2 * xdelta;
	 coords.w = coords.y - _y2 * ydelta;*/
	coords.x = _x1 * xdelta;
	coords.y = _y1 * ydelta;
	coords.z = coords.x + _x2 * xdelta;
	coords.w = coords.y + _y2 * ydelta;
	return coords;
}

void TextureGLES::Load(const char* _filepath)
{
	NSString* readPath = [[NSBundle mainBundle] resourcePath];
	char buffer[2048];
	[readPath getCString:buffer maxLength:2048 encoding:NSUTF8StringEncoding];
	
	char tbuff[2048];
	
	sprintf(tbuff, "%s/%s", buffer, _filepath);
	
	m_Filepath = tbuff;//readPath _filepath;
	
	Reload();
	
}

void TextureGLES::LoadData(const unsigned char* _datachunk, int _width, int _height, int _channels)
{
	m_Width = _width;
	m_Height = _height;
	m_Channels = _channels;
	
	if(_datachunk == NULL)
	{
		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}
	else
	{	
		m_TextureID = SOIL_create_OGL_texture(
											  _datachunk,
											  m_Width, m_Height, m_Channels,
											  SOIL_CREATE_NEW_ID,
											  SOIL_FLAG_MIPMAPS
											  );
	}
	
	if( m_TextureID == 0)
	{
		Message(LOCAL_MSG, "SOIL loading error data chunk: '%s';", SOIL_last_result() );
		return;
	}
}

void TextureGLES::Unload()
{
	glDeleteTextures( 1, &m_TextureID );
	m_TextureID = 0;
}

void TextureGLES::Reload()
{
	if (m_TextureID != 0)
	{
		Unload();
	}
	
	unsigned char* t_data = SOIL_load_image(m_Filepath.c_str(), &m_Width, &m_Height, &m_Channels, 0);
	if( t_data == 0)
	{
		Message(LOCAL_MSG, "SOIL data loading error for file '%s': '%s';", m_Filepath.c_str(), SOIL_last_result() );
		return;
	}
	
	m_TextureID = SOIL_create_OGL_texture(
										  t_data,
										  m_Width, m_Height, m_Channels,
										  SOIL_CREATE_NEW_ID,
										  NULL
										  );
	
	SOIL_free_image_data(t_data);
	
	if( m_TextureID == 0)
	{
		Message(LOCAL_MSG, "SOIL loading error for file '%s': '%s';", m_Filepath.c_str(), SOIL_last_result() );
		return;
	}
	
	Message(LOCAL_MSG, "Texture has been loaded: %s", m_Filepath.c_str());
}

void TextureGLES::SetMagFilter(TextureFilter _Filter)
{
	GLint param = GL_NEAREST;
	
	// use a lut
	if      (_Filter == TEX_FILTER_NEAREST) param = GL_NEAREST;
	else if (_Filter == TEX_FILTER_LINEAR)  param = GL_LINEAR;
	else    Message("TextureGL2", "invalid mag filter, using GL_NEAREST");
	
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param);
}

void TextureGLES::SetMinFilter(TextureFilter _Filter)
{
	GLint param = GL_NEAREST;
	
	// use a lut
	if      (_Filter == TEX_FILTER_NEAREST) param = GL_NEAREST;
	else if (_Filter == TEX_FILTER_LINEAR)  param = GL_LINEAR;
	else if (_Filter == TEX_FILTER_LINEAR_MIPMAP_LINEAR)  param = GL_LINEAR_MIPMAP_LINEAR;
	else if (_Filter == TEX_FILTER_LINEAR_MIPMAP_NEAREST)  param = GL_LINEAR_MIPMAP_NEAREST;
	else if (_Filter == TEX_FILTER_NEAREST_MIPMAP_LINEAR)  param = GL_NEAREST_MIPMAP_LINEAR;
	else if (_Filter == TEX_FILTER_NEAREST_MIPMAP_NEAREST)  param = GL_NEAREST_MIPMAP_NEAREST;
	else    Message("TextureGL2", "invalid mag filter, using GL_NEAREST");
	
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param);
}

void TextureGLES::SetClampMethod(TextureClampMethod _Method)
{
	GLint m = 0;
	switch(_Method)
	{
		case TEX_CLAMP: m = GL_CLAMP_TO_EDGE; break;
		case TEX_CLAMP_TO_EDGE: m = GL_CLAMP_TO_EDGE; break;
		case TEX_REPEAT: m = GL_REPEAT; break;
		default:
			PXFASSERT(false, "No such clamp method");
	}
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m);
}

