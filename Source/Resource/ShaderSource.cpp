#include <Pxf/Resource/ShaderSource.h>
#include <Pxf/Resource/Chunk.h>
#include <Pxf/Base/Utils.h>

#include <cstring>

using namespace Pxf;
using namespace Pxf::Resource;
using namespace Pxf::Util;

ShaderSource::ShaderSource(Chunk* _Chunk, const char* _Source)
	: AbstractResource(_Chunk, _Source)
	, m_ShaderSource(NULL)
{
	Build();
}

ShaderSource::~ShaderSource()
{
	SafeDelete(m_ShaderSource);
	SafeDelete(m_Chunk);
}

bool ShaderSource::Build()
{
	if (m_ShaderSource)
		SafeDelete(m_ShaderSource);

	char* Data = (char*)m_Chunk->Data;
	int _Size = m_Chunk->Size;
	Data = m_Chunk->Data;
	if (_Size < 12)
		return false;

	m_ShaderSource = new char[_Size];
	strncpy(m_ShaderSource, Data, _Size);
	m_ShaderSource[_Size] = 0;

	return true;
}
