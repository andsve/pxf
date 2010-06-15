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
}

void TextureGLES::Load(const char* _filepath)
{	
}

void TextureGLES::LoadData(const unsigned char* _datachunk, int _width, int _height, int _channels)
{
}

void TextureGLES::Unload()
{
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