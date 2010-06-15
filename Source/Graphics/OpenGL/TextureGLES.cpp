#include <Pxf/Graphics/OpenGL/TextureGLES.h>

using namespace Pxf;
using namespace Graphics;

TextureGLES::TextureGLES(Device* _pDevice) 
	: Texture(_pDevice)
{
	m_Filepath = 0;
	m_TextureID = 0;
}

TextureGLES::~TextureGLES()
{
	//
	Unload();
}

void TextureGLES::Load(const char* _filepath)
{	
	m_Filepath = _filepath;
	
	Reload();
}

void TextureGLES::LoadData(const unsigned char* _datachunk, int _width, int _height, int _channels)
{
	m_Width = _width;
	m_Height = _height;
	m_Channels = _channels;
	
	if(_datachunk == NULL)
	{
		// create empty texture
		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D,m_TextureID);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,_width,_height,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
		// Mimapping
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D,GL_GENERATE_MIPMAP,GL_TRUE);
	}
	else
	{
		/*m_TextureID = SOIL_create_OGL_texture (
			_datachunk,
			m_Width,m_Height,m_Channels,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS
		);*/
	}
	
	if(m_TextureID == 0)
	{
		//printf("SOIL loading error data chunk: '%s';", SOIL_last_result());
		return;
	}
		
}

void TextureGLES::Unload()
{
	glDeleteTextures(1,&m_TextureID);
	m_TextureID = 0;
}

void TextureGLES::Reload()
{
	
	
}

void TextureGLES::SetMagFilter(TextureFilter _Filter)
{
}
void TextureGLES::SetMinFilter(TextureFilter _Filter)
{
}

void TextureGLES::SetClampMethod(TextureClampMethod _Method)
{
}

Math::Vec4f TextureGLES::CreateTextureSubset(float _x1, float _y1, float _x2, float _y2)
{
	return 0;
}