#include <Pxf/Pxf.h>
#include <Pxf/Util/String.h>
#include <Pxf/Graphics/OpenGL/DeviceGL2.h>
#include <Pxf/Graphics/OpenGL/QuadBatchGL2.h>
#include <Pxf/Graphics/PrimitiveType.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>

#define LOCAL_MSG "QuadBatchGL2"

using namespace Pxf;
using namespace Pxf::Graphics;
using namespace Pxf::Math;
using Util::String;

QuadBatchGL2::QuadBatchGL2(int _maxSize)
{
	m_VertexBufferSize = _maxSize * 4; // _maxSize * number of vertices per quad
	m_Vertices = new Vertex[m_VertexBufferSize];

	m_VertexBufferPos = 0;
	m_CurrentDepthLayer = 0.5f;
	m_Rotation = 0.0f;
}

QuadBatchGL2::~QuadBatchGL2()
{
	SafeDeleteArray(m_Vertices);
}

void QuadBatchGL2::SetRotation(float angle)
{
	m_Rotation = angle;
}

void QuadBatchGL2::SetColor(float r, float g, float b, float a)
{
	m_CurrentColors[0].Set(r, g, b, a);
	m_CurrentColors[1].Set(r, g, b, a);
	m_CurrentColors[2].Set(r, g, b, a);
	m_CurrentColors[3].Set(r, g, b, a);
}

void QuadBatchGL2::SetColor(Vec4f* c)
{
	SetColor(c->r, c->g, c->b, c->a);
}

void QuadBatchGL2::SetTextureSubset(float tl_u, float tl_v, float br_u, float br_v)
{
	m_CurrentTexCoords[0].u = tl_u;
	m_CurrentTexCoords[0].v = tl_v;

	m_CurrentTexCoords[1].u = br_u;
	m_CurrentTexCoords[1].v = tl_v;

	m_CurrentTexCoords[2].u = br_u;
	m_CurrentTexCoords[2].v = br_v;

	m_CurrentTexCoords[3].u = tl_u;
	m_CurrentTexCoords[3].v = br_v;
}

void QuadBatchGL2::Rotate(const Math::Vec3f &center, Math::Vec3f &point)
{
	Math::Vec3f p = point - center;
	point.x = p.x * cosf(m_Rotation) - p.y * sinf(m_Rotation) + center.x;
	point.y = p.x * sinf(m_Rotation) + p.y * cosf(m_Rotation) + center.y;
}

void QuadBatchGL2::Reset()
{
	m_VertexBufferPos = 0;
	SetRotation(0.f);
	SetTextureSubset(0.f,0.f,1.f,1.f);
	SetColor(1.f,1.f,1.f,1.f);	
}

void QuadBatchGL2::AddFreeform(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3)
{
	if (m_VertexBufferPos >= m_VertexBufferSize)
	{
		Message(LOCAL_MSG, "Not enough space in vertex array! Try increasing _maxSize in QuadBatch constructor.");
		return;
	}

	m_Vertices[m_VertexBufferPos].pos.x = x0;
	m_Vertices[m_VertexBufferPos].pos.y = y0;
	m_Vertices[m_VertexBufferPos].pos.z = m_CurrentDepthLayer;
	m_Vertices[m_VertexBufferPos].tex = m_CurrentTexCoords[3];
	m_Vertices[m_VertexBufferPos].color = m_CurrentColors[3];

	m_Vertices[m_VertexBufferPos+3].pos.x = x1;
	m_Vertices[m_VertexBufferPos+3].pos.y = y1;
	m_Vertices[m_VertexBufferPos+3].pos.z = m_CurrentDepthLayer;
	m_Vertices[m_VertexBufferPos+3].tex = m_CurrentTexCoords[3];
	m_Vertices[m_VertexBufferPos+3].color = m_CurrentColors[3];

	m_Vertices[m_VertexBufferPos+3].pos.x = x2;
	m_Vertices[m_VertexBufferPos+3].pos.y = y2;
	m_Vertices[m_VertexBufferPos+3].pos.z = m_CurrentDepthLayer;
	m_Vertices[m_VertexBufferPos+3].tex = m_CurrentTexCoords[3];
	m_Vertices[m_VertexBufferPos+3].color = m_CurrentColors[3];

	m_Vertices[m_VertexBufferPos+3].pos.x = x3;
	m_Vertices[m_VertexBufferPos+3].pos.y = y3;
	m_Vertices[m_VertexBufferPos+3].pos.z = m_CurrentDepthLayer;
	m_Vertices[m_VertexBufferPos+3].tex = m_CurrentTexCoords[3];
	m_Vertices[m_VertexBufferPos+3].color = m_CurrentColors[3];

	m_VertexBufferPos += 4;
}

void QuadBatchGL2::AddTopLeft(float x, float y, float w, float h)
{
	if (m_VertexBufferPos >= m_VertexBufferSize)
	{
		Message(LOCAL_MSG, "Not enough space in vertex array! Try increasing _maxSize in QuadBatch constructor.");
		return;
	}

	Math::Vec3f center(x + w/2, y + h/2, 0.f);

	m_Vertices[m_VertexBufferPos].pos.x = x;
	m_Vertices[m_VertexBufferPos].pos.y = y;
	m_Vertices[m_VertexBufferPos].tex = m_CurrentTexCoords[0];
	m_Vertices[m_VertexBufferPos].color = m_CurrentColors[0];
	Rotate(center, m_Vertices[m_VertexBufferPos].pos);

	m_Vertices[m_VertexBufferPos+1].pos.x = x+w;
	m_Vertices[m_VertexBufferPos+1].pos.y = y;
	m_Vertices[m_VertexBufferPos+1].tex = m_CurrentTexCoords[1];
	m_Vertices[m_VertexBufferPos+1].color = m_CurrentColors[1];
	Rotate(center, m_Vertices[m_VertexBufferPos+1].pos);

	m_Vertices[m_VertexBufferPos+2].pos.x = x+w;
	m_Vertices[m_VertexBufferPos+2].pos.y = y+h;
	m_Vertices[m_VertexBufferPos+2].tex = m_CurrentTexCoords[2];
	m_Vertices[m_VertexBufferPos+2].color = m_CurrentColors[2];
	Rotate(center, m_Vertices[m_VertexBufferPos+2].pos);

	m_Vertices[m_VertexBufferPos+3].pos.x = x;
	m_Vertices[m_VertexBufferPos+3].pos.y = y+h;
	m_Vertices[m_VertexBufferPos+3].tex = m_CurrentTexCoords[3];
	m_Vertices[m_VertexBufferPos+3].color = m_CurrentColors[3];
	Rotate(center, m_Vertices[m_VertexBufferPos+3].pos);

	m_VertexBufferPos += 4;
}

void QuadBatchGL2::AddCentered(float x, float y, float w, float h)
{
	AddTopLeft(x-w/2, y-h/2, w, h);
}

void QuadBatchGL2::Draw()
{
	Flush();
}

void QuadBatchGL2::Flush()
{
	if (m_VertexBufferPos == 0)
		return;

	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (char*)m_Vertices);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (char*)m_Vertices + sizeof(float) * 3);
	glColorPointer(4, GL_FLOAT, sizeof(Vertex), (char*)m_Vertices + sizeof(float) * 5);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	
	glDrawArrays(GL_QUADS, 0, m_VertexBufferPos);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}
