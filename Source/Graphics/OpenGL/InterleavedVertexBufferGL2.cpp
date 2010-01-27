#include <Pxf/Pxf.h>
#include <Pxf/Util/String.h>
#include <Pxf/Graphics/OpenGL/InterleavedVertexBufferGL2.h>
#include <Pxf/Graphics/PrimitiveType.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>

#include <Pxf/Graphics/OpenGL/OpenGL.h>
#include <Pxf/Graphics/OpenGL/TypeTraits.h>

#define LOCAL_MSG "VertexBufferGL"

using namespace Pxf;
using namespace Pxf::Graphics;
using Util::String;




InterleavedVertexBufferGL2::InterleavedVertexBufferGL2(VertexBufferLocation _VertexBufferLocation)
	: InterleavedVertexBuffer(_VertexBufferLocation)
{

}

InterleavedVertexBufferGL2::~InterleavedVertexBufferGL2()
{

}


void InterleavedVertexBufferGL2::_PreDraw()
{
	// VB_ATTRIB_DATA does not need to be handled?


	PXFASSERT(m_Attributes & VB_VERTEX_DATA, "Attempt to draw without vertex data.");
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(m_VertexAttributes.TypeSize, GL_FLOAT, m_VertexSize, GL::BufferObjectPtr(m_VertexAttributes.StrideOffset));


	if(m_Attributes & VB_NORMAL_DATA)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
	}

	if(m_Attributes & VB_TEXCOORD_DATA)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	if(m_Attributes & VB_COLOR_DATA)
	{
		glEnableClientState(GL_COLOR_ARRAY);
	}

	if(m_Attributes & VB_INDEX_DATA)
	{
		glEnableClientState(GL_INDEX_ARRAY);
	}

	if(m_Attributes & VB_EDGEFLAG_DATA)
	{
		glEnableClientState(GL_EDGE_FLAG_ARRAY);
	}
}

void InterleavedVertexBufferGL2::_PostDraw()
{
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

	if(m_Attributes & VB_INDEX_DATA)
	{
		glDisableClientState(GL_INDEX_ARRAY);
	}

	if(m_Attributes & VB_EDGEFLAG_DATA)
	{
		glDisableClientState(GL_EDGE_FLAG_ARRAY);
	}
}


/*
void InterleavedVertexBufferGL2::SetData(VertexBufferAttribute _AttribType, unsigned _TypeSize, const void* _Ptr, unsigned _Count, unsigned _Stride)
{
	m_Attributes |= _AttribType;

	if (_AttribType == VB_VERTEX_DATA)
	{
		
	}

	else if(_AttribType == VB_NORMAL_DATA)
	{
		PXFASSERT(_TypeSize == 3, "Wrong size specified for normal data");
		glNormalPointer(GL_FLOAT, _Stride, _Ptr);
	}

	else if(_AttribType == VB_TEXCOORD_DATA)
	{
		glTexCoordPointer(_TypeSize, GL_FLOAT, _Stride, _Ptr);
	}

	else if(_AttribType == VB_COLOR_DATA)
	{
		glVertexPointer(_TypeSize, GL_FLOAT, _Stride, _Ptr);
	}

	else if(_AttribType == VB_INDEX_DATA)
	{
		glIndexPointer(GL_FLOAT, _Stride, _Ptr);
	}

	else if(_AttribType == VB_EDGEFLAG_DATA)
	{
		glEdgeFlagPointer(_Stride, _Ptr);
	}

	else if(_AttribType == VB_ATTRIB_DATA)
	{
		PXFASSERT(0, "Not implemented.");
	}
}
*/

void* InterleavedVertexBufferGL2::CreateNewBuffer(uint32 _NumVertices, uint32 _VertexSize, VertexBufferUsageFlag _UsageFlag)
{
	m_VertexSize = _VertexSize;
	m_VertexBufferUsageFlag = _UsageFlag;
	return NULL;
}

void InterleavedVertexBufferGL2::SetBuffer(void* _Buffer,uint32 _NumVertices, uint32 _VertexSize, VertexBufferUsageFlag _UsageFlag)
{
	m_VertexSize = _VertexSize;
	m_VertexBufferUsageFlag = _UsageFlag;
}

void InterleavedVertexBufferGL2::UpdateData(void* _Buffer, uint32 _Count, uint32 _Offset)
{

}

void* InterleavedVertexBufferGL2::MapData(VertexBufferAccessFlag _AccessFlag)
{
	return NULL;
}

void InterleavedVertexBufferGL2::UnmapData()
{

}

bool InterleavedVertexBufferGL2::Commit()
{
	return false;
}

