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

	m_TextureID = SOIL_load_OGL_texture(
		m_Filepath.c_str(),
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS
		);

	if( m_TextureID == 0)
	{
		Message(LOCAL_MSG, "SOIL loading error for file '%s': '%s';", m_Filepath.c_str(), SOIL_last_result() );
		return;
	}
}

GLuint TextureGL2::GetTextureID()
{
	return m_TextureID;
}
