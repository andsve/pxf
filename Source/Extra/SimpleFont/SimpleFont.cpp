#include <fstream>

#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Stream.h>
#include <Pxf/Graphics/Device.h>
#include <Pxf/Graphics/Texture.h>
#include <Pxf/Graphics/QuadBatch.h>
#include <Pxf/Extra/SimpleFont/SimpleFont.h>

#if defined(CONF_PLATFORM_MACOSX)
	#include "sys/malloc.h" // mac os x
#else
	#include "malloc.h" // linux, windows
#endif

#define STB_TRUETYPE_IMPLEMENTATION
#include <Pxf/Extra/SimpleFont/stb_truetype.h>

#define LOCAL_MSG "SimpleFont"

using namespace Pxf;
using namespace Pxf::Graphics;
using namespace Pxf::Extra;

SimpleFont::SimpleFont(Device *_device)
{
	m_Device = _device;
	
	m_QuadBatch = m_Device->CreateQuadBatch(PXF_EXTRA_SIMPLEFONT_MAXQUAD_PER_FONT);
	m_QuadBatch->Reset();
	//m_QuadBatch->SetTextureSubset(0, 0, 1, 1);
	//m_QuadBatch->AddTopLeft(0, 0, m_TextureSize, m_TextureSize);
}

SimpleFont::~SimpleFont()
{
	if (m_QuadBatch)
		delete m_QuadBatch;
}

void SimpleFont::ResetText()
{
	m_QuadBatch->Reset();
}

void SimpleFont::Load(Util::String _font_filepath, float _font_size, int _texture_size)
{
	m_FontFilepath = _font_filepath;
	m_TextureSize = _texture_size;
	m_FontSize = _font_size;
	
	FileStream fs;
	if (fs.OpenReadBinary(m_FontFilepath.c_str()))
	{
		int filesize = fs.GetSize();
		unsigned char *ttf_buffer = new unsigned char[filesize];                    // store ttf data
		unsigned char *temp_bitmap = new unsigned char[m_TextureSize*m_TextureSize]; // store texture pixels
		
		// read ttf data
		fs.Read(ttf_buffer, filesize);
		
		// bake ttf font data
		stbtt_BakeFontBitmap(ttf_buffer, 0, m_FontSize, temp_bitmap, m_TextureSize, m_TextureSize, 32, 96, m_CharData);
		delete ttf_buffer;
		
		// create texture
		m_CharmapTexture = m_Device->CreateTextureFromData(temp_bitmap, m_TextureSize, m_TextureSize, 1);
		delete temp_bitmap;
		
		// set some texture filtering
		m_CharmapTexture->SetMagFilter(TEX_FILTER_NEAREST);
		m_CharmapTexture->SetMinFilter(TEX_FILTER_NEAREST);
	} else {
		Message(LOCAL_MSG, "Could not open font: %s", m_FontFilepath.c_str());
	}
}

void SimpleFont::AddTextCentered(Util::String _text, Math::Vec3f _pos)
{
	const char *text = _text.c_str();
	float _width = 0.0f;
	float _x = _pos.x;
	float _y = _pos.y;
	
	while (*text) {
		if (*text >= 32 && *text < 128) {
			stbtt_aligned_quad q;
			stbtt_GetBakedQuad(m_CharData, m_TextureSize, m_TextureSize, *text-32, &_x,&_y,&q,1);//1=opengl,0=old d3d
			
			_width += q.x1 - q.x0;
		}
		++text;
	}
	
	AddText(_text, Math::Vec3f(_pos.x - (_width / 2.0f), _pos.y, _pos.x));
}

void SimpleFont::AddText(Util::String _text, Math::Vec3f _pos)
{
	const char *text = _text.c_str();
	float _cell_offset = 0.0f;
	float _x = _pos.x;
	float _y = _pos.y;
	
	while (*text) {
		if (*text >= 32 && *text < 128) {
			stbtt_aligned_quad q;
			stbtt_GetBakedQuad(m_CharData, m_TextureSize, m_TextureSize, *text-32, &_x,&_y,&q,1);//1=opengl,0=old d3d
			
			m_QuadBatch->SetTextureSubset(q.s0, q.t0, q.s1, q.t1);
			m_QuadBatch->AddTopLeft(q.x0, q.y0, q.x1 - q.x0, q.y1 - q.y0);
		}
		_cell_offset += m_FontSize;
		++text;
	}

}

void SimpleFont::Draw()
{
	m_Device->BindTexture(m_CharmapTexture);
	m_CharmapTexture->SetMagFilter(TEX_FILTER_NEAREST);
	m_CharmapTexture->SetMinFilter(TEX_FILTER_NEAREST);
	m_QuadBatch->Draw();
}