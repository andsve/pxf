
#include <Pxf/Pxf.h>
#include <Pxf/Graphics/OpenGL/OpenGL.h>
#include <Pxf/Util/String.h>
#include <Pxf/Graphics/OpenGL/VertexBufferGLES11.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/Stream.h>

#define LOCAL_MSG "VertexBufferGLES11"

using namespace Pxf;
using namespace Pxf::Graphics;
using Util::String;

static GLuint LookupUsageFlag(VertexBufferUsageFlag _BufferUsageFlag)
{
	switch(_BufferUsageFlag)
	{
		case VB_USAGE_STATIC_DRAW: return GL_STATIC_DRAW;
		case VB_USAGE_DYNAMIC_DRAW: return GL_DYNAMIC_DRAW;
		default: printf("VertexBufferGLES11: Usage Mode not supported in this context\n");
	}
	return 0;
}

static GLuint LookupAccessFlag(VertexBufferAccessFlag _BufferAccessFlag)
{
	switch(_BufferAccessFlag)
	{
		case VB_ACCESS_WRITE_ONLY: return GL_WRITE_ONLY_OES;
		default: printf("VertexBufferGLES11: Access Mode not supported in this context\n");
	}

	return 0;
}


VertexBufferGLES11::VertexBufferGLES11(Device* _pDevice, VertexBufferLocation _VertexBufferLocation, VertexBufferUsageFlag _VertexBufferUsageFlag)
	: VertexBuffer(_pDevice, _VertexBufferLocation, _VertexBufferUsageFlag)
	, m_BufferObjectId(0)
{}

VertexBufferGLES11::~VertexBufferGLES11()
{
	if (m_VertexBufferLocation == VB_LOCATION_GPU)
	{
		if (m_BufferObjectId)
			glDeleteBuffers(1, (GLuint*) &m_BufferObjectId);
	}
	else
	{
		if (m_InterleavedData)
			delete [] m_InterleavedData;
	}
}


void VertexBufferGLES11::_PreDraw()
{
	unsigned int BufferOffset = 0;
	if (m_VertexBufferLocation == VB_LOCATION_GPU)
	{
		glBindBuffer(GL_ARRAY_BUFFER, (GLuint) m_BufferObjectId);
	}
	else
	{
		BufferOffset = (unsigned int)m_InterleavedData;
	}

	PXFASSERT(m_Attributes & VB_VERTEX_DATA, "Attempt to draw without vertex data.");
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(m_VertexAttributes.NumComponents, GL_FLOAT, m_VertexSize,(GLvoid*)( ((char*)NULL) + BufferOffset + m_VertexAttributes.StrideOffset));


	if(m_Attributes & VB_NORMAL_DATA)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, m_VertexSize, ((char*)NULL) + BufferOffset + m_NormalAttributes.StrideOffset);
	}

	if(m_Attributes & VB_TEXCOORD_DATA)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(m_TexCoordAttributes.NumComponents, GL_FLOAT, m_VertexSize,(GLvoid*)( ((char*)NULL) + BufferOffset + m_TexCoordAttributes.StrideOffset));
	}

	if(m_Attributes & VB_COLOR_DATA)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(m_ColorAttributes.NumComponents, GL_FLOAT, m_VertexSize, (GLvoid*)( ((char*)NULL) + BufferOffset + m_ColorAttributes.StrideOffset));
	}

	/*
	if(m_Attributes & VB_INDEX_DATA)
	{
		glEnableClientState(GL_INDEX_ARRAY);
		glIndexPointer(GL_FLOAT, m_VertexSize,(GLvoid*)( ((char*)NULL) + BufferOffset + m_ColorAttributes.StrideOffset));
	}

	if(m_Attributes & VB_EDGEFLAG_DATA)
	{
		glEnableClientState(GL_EDGE_FLAG_ARRAY);
		glEdgeFlagPointer(m_VertexSize, GL::BufferObjectPtr(BufferOffset + m_EdgeFlagAttributes.StrideOffset));
	}*/
}

void VertexBufferGLES11::_PostDraw()
{
	if (m_VertexBufferLocation == VB_LOCATION_GPU)
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	glDisableClientState(GL_VERTEX_ARRAY);


	if(m_Attributes & VB_NORMAL_DATA)
	{
		glDisableClientState(GL_NORMAL_ARRAY);
	}

	if(m_Attributes & VB_TEXCOORD_DATA)
	{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	if(m_Attributes & VB_COLOR_DATA)
	{
		glDisableClientState(GL_COLOR_ARRAY);
	}
/*
	if(m_Attributes & VB_INDEX_DATA)
	{
		glDisableClientState(GL_INDEX_ARRAY);
	}

	if(m_Attributes & VB_EDGEFLAG_DATA)
	{
		glDisableClientState(GL_EDGE_FLAG_ARRAY);
	}*/
}



void VertexBufferGLES11::CreateNewBuffer(uint32 _NumVertices, uint32 _VertexSize)
{
	if (m_InterleavedData != 0 || m_BufferObjectId != 0)
		return;

	if (m_VertexBufferLocation == VB_LOCATION_GPU)
	{
		GLuint usage = LookupUsageFlag(m_VertexBufferUsageFlag);
		glGenBuffers(1, (GLuint*)&m_BufferObjectId);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferObjectId);
		glBufferData(GL_ARRAY_BUFFER, _NumVertices * _VertexSize,0,usage);
			

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	else
	{
		m_InterleavedData = new char[_NumVertices * _VertexSize];
	}

	m_VertexCount = _NumVertices;
	m_VertexSize = _VertexSize;
	m_ByteCount = _NumVertices * _VertexSize;
}

void VertexBufferGLES11::CreateFromBuffer(void* _Buffer,uint32 _NumVertices, uint32 _VertexSize)
{
	if (m_InterleavedData != 0 && m_BufferObjectId != 0)
		return;
	
	if (m_VertexBufferUsageFlag == VB_LOCATION_GPU)
	{
		// Copy to gpu memory
		GLuint usage = LookupUsageFlag(m_VertexBufferUsageFlag);
		glBindBuffer(GL_ARRAY_BUFFER, (GLuint) m_BufferObjectId);
		glBufferData(GL_ARRAY_BUFFER, _NumVertices * _VertexSize, _Buffer, usage);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	else
	{
		m_InterleavedData = new char[_NumVertices * _VertexSize];
		MemoryStream stream((char*)m_InterleavedData, _NumVertices * _VertexSize);
		stream.Write(_Buffer, _NumVertices * _VertexSize);
	}

	m_VertexCount = _NumVertices;
	m_VertexSize = _VertexSize;
	m_ByteCount = _NumVertices * _VertexSize;
}

void VertexBufferGLES11::UpdateData(void* _Buffer, uint32 _Count, uint32 _Offset)
{
	if (m_VertexBufferLocation == VB_LOCATION_GPU && m_BufferObjectId != 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, (GLuint) m_BufferObjectId);
		glBufferSubData(GL_ARRAY_BUFFER, _Offset, _Count, _Buffer);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	else if (m_VertexBufferLocation == VB_LOCATION_SYS && m_InterleavedData != 0)
	{
		MemoryStream stream((char*)m_InterleavedData, m_ByteCount);
		stream.SeekTo(_Offset);
		stream.Write(_Buffer, _Count);
	}
}

void* VertexBufferGLES11::MapData(VertexBufferAccessFlag _AccessFlag)
{
	if (m_VertexBufferLocation == VB_LOCATION_GPU && m_BufferObjectId != 0)
	{
		GLuint access = LookupAccessFlag(_AccessFlag);
		glBindBuffer(GL_ARRAY_BUFFER, (GLuint) m_BufferObjectId);
		void* data = glMapBufferOES(GL_ARRAY_BUFFER, access);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		return data;
	}
	else if (m_VertexBufferLocation == VB_LOCATION_SYS && m_InterleavedData != 0)
	{
		m_IsMapped = true;
		return m_InterleavedData;
	}

	return NULL;
}

void VertexBufferGLES11::UnmapData()
{

	if (m_VertexBufferLocation == VB_LOCATION_GPU && m_BufferObjectId != 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, (GLuint) m_BufferObjectId);
		glUnmapBufferOES(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		m_IsMapped = false;
	}
	
	if (m_VertexBufferLocation == VB_LOCATION_SYS && m_InterleavedData != 0)
	{
		m_IsMapped = false;
	}
}

