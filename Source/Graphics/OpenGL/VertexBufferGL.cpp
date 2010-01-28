#include <Pxf/Pxf.h>
#include <Pxf/Util/String.h>
#include <Pxf/Graphics/OpenGL/VertexBufferGL.h>
#include <Pxf/Graphics/Device.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>

#include <Pxf/Graphics/OpenGL/OpenGL.h>

#define LOCAL_MSG "VertexBufferGL"

using namespace Pxf;
using namespace Pxf::Graphics;
using Util::String;

VertexBufferGL::VertexBufferGL(void *_Data, unsigned int _Offset, unsigned int _VerticeCount, bool _Dynamic)
{
	//
	if(_Data)
		m_VBuffer	= _Data;
	else
		Message(LOCAL_MSG,"Unable to create Vertex Buffer with empty data");

	m_Stride		= _Offset;
	m_VCount		= _VerticeCount;
	m_PrimitiveMode	= 0;
	m_DynamicDraw	= _Dynamic;
	CreateVBO();
}

VertexBufferGL::~VertexBufferGL()
{
	// clean up
	SafeDeleteArray(m_VBuffer);
	DestroyVBO();
}

bool VertexBufferGL::DestroyVBO()
{
	Unbind();
	glDeleteBuffersARB(1,&m_VBOID);
	return true;
}

bool VertexBufferGL::CreateVBO()
{
	glGenBuffersARB(1, &m_VBOID);
	
	if(!IsBound())
	{
		Bind();
		// upload data
		// need to know vertex format before we can write the data to the graphics card..
		
		/*
		if(m_DynamicDraw)
			glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeInBytes, m_VBuffer, GL_DYNAMIC_DRAW_ARB);
		else
			glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeInBytes, m_VBuffer, GL_STATIC_DRAW_ARB);
		*/
		Unbind();
		
		return true;
	}
	else
	{
		Message(LOCAL_MSG,"Unable to write data to a bound buffer");
		return false;
	}
}

void VertexBufferGL::Bind()
{
	if(IsBound())
		return;

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_VBOID);
	m_Bound = true;
}

void VertexBufferGL::Unbind()
{
	if(!IsBound())
		return;

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	m_Bound = false;
}

VertexBufferPrimitiveType VertexBufferGL::GetPrimitive()
{
	switch(m_PrimitiveMode)
	{
		// might be wrong or something missing
		case(GL_POINTS): return VB_PRIMITIVE_POINTS;
		case(GL_LINES):	return VB_PRIMITIVE_LINES;
		case(GL_LINE_STRIP): return VB_PRIMITIVE_LINE_STRIP;
		case(GL_TRIANGLES): return VB_PRIMITIVE_TRIANGLES;
		case(GL_TRIANGLE_STRIP): return VB_PRIMITIVE_TRIANGLE_STRIP;
		case(GL_TRIANGLE_FAN): return VB_PRIMITIVE_TRIANGLE_FAN;
	}

	// default enum (is there a better solution?:E)
	return VB_PRIMITIVE_NONE;
}

void VertexBufferGL::SetPrimitive(VertexBufferPrimitiveType _PrimitiveType)
{
	switch(_PrimitiveType)
	{
		case(VB_PRIMITIVE_POINTS): m_PrimitiveMode = GL_POINTS;
		case(VB_PRIMITIVE_LINES): m_PrimitiveMode = GL_LINES;
		case(VB_PRIMITIVE_LINE_STRIP): m_PrimitiveMode = GL_LINE_STRIP;
		case(VB_PRIMITIVE_TRIANGLES): m_PrimitiveMode = GL_TRIANGLES;
		case(VB_PRIMITIVE_TRIANGLE_STRIP): m_PrimitiveMode = GL_TRIANGLE_STRIP;
		case(VB_PRIMITIVE_TRIANGLE_FAN): m_PrimitiveMode = GL_TRIANGLE_FAN;
	}
}



