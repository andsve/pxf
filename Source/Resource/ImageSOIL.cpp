#include <Pxf/Resource/Image.h>
//#include <Pxf/Resource/Resource.h>
#include <Pxf/Resource/Chunk.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>

//#include <SOIL.h>
#include "SOIL.h"

using namespace Pxf::Resource;
using namespace Pxf::Util;

Image::Image(Chunk* _Chunk, const char* _Source)
	: AbstractResource(_Chunk, _Source)
	, m_Channels(0)
	, m_Height(0)
	, m_Width(0)
	, m_ImageData(NULL)
{
	Build();
}

Image::~Image()
{
	if(m_ImageData)
		SOIL_free_image_data(m_ImageData);
	
	SafeDelete(m_Chunk);
}

bool Image::Build()
{
	if(m_ImageData)
		SOIL_free_image_data(m_ImageData);

	// load raw data into memory
	m_ImageData = SOIL_load_image_from_memory(reinterpret_cast<const unsigned char*>(m_Chunk->Data), m_Chunk->Size, &m_Width, &m_Height, &m_Channels, 0);

	if (!m_ImageData)
	{
		const char* error = SOIL_last_result();
		Message("Image", "failed to create image from '%s'", m_Source.c_str());
		Message("Image", error);
		return false;
	}
	return true;
}
