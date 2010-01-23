#include <Pxf/Pxf.h>
#include <Pxf/Util/String.h>
#include <Pxf/Graphics/OpenGL/VertexBufferGL2_VA.h>
#include <Pxf/Graphics/PrimitiveType.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>

#include <Pxf/Graphics/OpenGL/OpenGL.h>

#define LOCAL_MSG "VertexBufferGL"

using namespace Pxf;
using namespace Pxf::Graphics;
using Util::String;

VertexBufferGL2_VA::VertexBufferGL2_VA(VertexBufferType _VertexBufferType)
	: VertexBuffer(_VertexBufferType)
{

}

VertexBufferGL2_VA::~VertexBufferGL2_VA()
{

}

void VertexBufferGL2_VA::_PreDraw()
{

}

void VertexBufferGL2_VA::_PostDraw()
{

}


void VertexBufferGL2_VA::SetData(VertexBufferAttribute _AttribType, unsigned _TypeSize, const void* _Ptr, unsigned _Count, unsigned _Stride)
{

}

bool VertexBufferGL2_VA::Commit()
{
	return false;
}

