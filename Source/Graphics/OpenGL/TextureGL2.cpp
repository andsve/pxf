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

Math::Vec4i TextureGL2::CreateTextureSubset(float _x1, float _y1, float _x2, float _y2)
{
	float xdelta, ydelta;
	xdelta = 1.0f / m_Width;
	ydelta = 1.0f / m_Height;

	Math::Vec4i coords;
	coords.x = _x1 * xdelta;
	coords.y = _y1 * ydelta;
	coords.z = _x2 * xdelta;
	coords.w = _y2 * ydelta;
	return coords;
}

void TextureGL2::Load(const char* _filepath)
{
	m_Filepath = _filepath;

	Reload();

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
		SOIL_FLAG_MIPMAPS
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
