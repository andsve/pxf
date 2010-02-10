#include <Pxf/Pxf.h>
#include <Pxf/Util/String.h>
#include <Pxf/Graphics/OpenGL/TextureGL2.h>
#include <Pxf/Base/Debug.h>

#include <SOIL.h>
#include <GL/glfw.h>

#define LOCAL_MSG "TextureGL2"

using namespace Pxf;
using namespace Pxf::Graphics;
using Util::String;

TextureGL2::TextureGL2()
{
	m_TextureID = 0;
}

TextureGL2::~TextureGL2()
{
	Unload();
}

Math::Vec4f TextureGL2::CreateTextureSubset(float _x1, float _y1, float _x2, float _y2)
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

void TextureGL2::Load(const char* _filepath)
{
	m_Filepath = _filepath;

	Reload();

}

void TextureGL2::LoadData(const unsigned char* _datachunk, int _width, int _height, int _channels)
{
	m_Width = _width;
	m_Height = _height;
	m_Channels = _channels;
	
	/*
	GLuint tformat;
	
	if (_format == FORMAT_RGBA)
	{
		tformat = GL_RGBA;
	} else if (_format == FORMAT_A)
	{
		tformat = GL_ALPHA;
	}
	
	
	// doing it old-school
	glGenTextures(1, &m_TextureID);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, tformat, _width, _height, 0, tformat, GL_UNSIGNED_BYTE, _datachunk);
	glGenerateMipmapEXT(GL_TEXTURE_2D);
	*/
	
	
	m_TextureID = SOIL_create_OGL_texture(
		_datachunk,
		m_Width, m_Height, m_Channels,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS
		);
	
	if( m_TextureID == 0)
	{
		Message(LOCAL_MSG, "SOIL loading error data chunk: '%s';", SOIL_last_result() );
		return;
	}
}

void TextureGL2::Unload()
{
	glDeleteTextures( 1, &m_TextureID );
	m_TextureID = 0;
}

void TextureGL2::Reload()
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
}

int TextureGL2::GetWidth()
{
	return m_Width;
}

int TextureGL2::GetHeight()
{
	return m_Height;
}

int TextureGL2::GetChannels()
{
	return m_Channels;
}

GLuint TextureGL2::GetTextureID()
{
	return m_TextureID;
}


void TextureGL2::SetMagFilter(TextureFilter _Filter)
{
	GLint param = GL_NEAREST;

	// use a lut
	if      (_Filter == FILTER_NEAREST) param = GL_NEAREST;
	else if (_Filter == FILTER_LINEAR)  param = GL_LINEAR;
	else    Message("TextureGL2", "invalid mag filter, using GL_NEAREST");
	
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param);
}

void TextureGL2::SetMinFilter(TextureFilter _Filter)
{
	GLint param = GL_NEAREST;

	// use a lut
	if      (_Filter == FILTER_NEAREST) param = GL_NEAREST;
	else if (_Filter == FILTER_LINEAR)  param = GL_LINEAR;
	else if (_Filter == FILTER_LINEAR_MIPMAP_LINEAR)  param = GL_LINEAR_MIPMAP_LINEAR;
	else if (_Filter == FILTER_LINEAR_MIPMAP_NEAREST)  param = GL_LINEAR_MIPMAP_NEAREST;
	else if (_Filter == FILTER_NEAREST_MIPMAP_LINEAR)  param = GL_NEAREST_MIPMAP_LINEAR;
	else if (_Filter == FILTER_NEAREST_MIPMAP_NEAREST)  param = GL_NEAREST_MIPMAP_NEAREST;
	else    Message("TextureGL2", "invalid mag filter, using GL_NEAREST");

	glBindTexture(GL_TEXTURE_2D, m_TextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param);
}

void TextureGL2::SetClampMethod(TextureClampMethod _Method)
{
	GLint m = 0;
	switch(_Method)
	{
	case CLAMP: m = GL_CLAMP; break;
	case CLAMP_TO_EDGE: m = GL_CLAMP_TO_EDGE; break;
	case REPEAT: m = GL_REPEAT; break;
	default:
		PXFASSERT(false, "No such clamp method");
	}
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m);
}
