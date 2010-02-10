#include <fstream>

#include <Pxf/Base/Debug.h>
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

SimpleFont::SimpleFont( Util::String _font_filepath, Device *_device)
{
	m_Device = _device;
	m_FontFilepath = _font_filepath;
	m_TextureSize = 256;
	m_FontSize = 13.0f;
	
	m_QuadBatch = m_Device->CreateQuadBatch(PXF_EXTRA_SIMPLEFONT_MAXQUAD_PER_FONT);
	m_QuadBatch->Reset();
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

void SimpleFont::Load()
{
	FILE *_ttf_file;
	_ttf_file = fopen (m_FontFilepath.c_str(),"r");
	if (_ttf_file != NULL)
	{
		int _filesize = _get_filesize();
		unsigned char *_ttf_buffer = new unsigned char[_filesize];                    // store ttf data
		unsigned char *_temp_bitmap = new unsigned char[m_TextureSize*m_TextureSize]; // store texture pixels
		
		// read ttf data
		fread(_ttf_buffer, 1, _filesize, _ttf_file);
		fclose (_ttf_file);
		
		// bake ttf font data
		stbtt_BakeFontBitmap(_ttf_buffer, 0, m_FontSize, _temp_bitmap, m_TextureSize, m_TextureSize, 32, 96, m_CharData);
		delete _ttf_buffer;
		
		// create texture
		m_CharmapTexture = m_Device->CreateTextureFromData(_temp_bitmap, m_TextureSize, m_TextureSize, 1, FORMAT_A);
		delete _temp_bitmap;
		
		// set some texture filtering
		m_CharmapTexture->SetMagFilter(FILTER_NEAREST);
		m_CharmapTexture->SetMinFilter(FILTER_NEAREST);
	} else {
		Message(LOCAL_MSG, "Could not open font: %s", m_FontFilepath.c_str());
	}
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
	m_QuadBatch->Draw();
}

// Portable file size getter from:
// http://www.codeproject.com/KB/files/filesize.aspx
int SimpleFont::_get_filesize()
{
	std::ifstream f;
	f.open(m_FontFilepath.c_str(), std::ios_base::binary | std::ios_base::in);
	
	if (!f.good() || f.eof() || !f.is_open()) { return 0; }
		f.seekg(0, std::ios_base::beg);
		
	std::ifstream::pos_type begin_pos = f.tellg();
	f.seekg(0, std::ios_base::end);
	return static_cast<int>(f.tellg() - begin_pos);
}
