#include <Pxf/Pxf.h>
#include <Pxf/Util/String.h>
#include <Pxf/Graphics/Device.h>
#include <Pxf/Graphics/OpenGL/QuadBatchGLES11.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>

#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>

#define LOCAL_MSG "QuadBatchGLES11"

using namespace Pxf;
using namespace Pxf::Graphics;
using namespace Pxf::Math;
using Util::String;

QuadBatchGLES11::QuadBatchGLES11(Device* _pDevice, int _maxSize)
	: QuadBatch(_pDevice)
{
	m_VertexBufferSize = _maxSize * 6; // _maxSize * number of vertices per quad
	m_Vertices = new Vertex[m_VertexBufferSize];

	m_VertexBufferPos = 0;
	m_CurrentDepthLayer = 0.5f;
	
}

QuadBatchGLES11::~QuadBatchGLES11()
{
	SafeDeleteArray(m_Vertices);
}

void QuadBatchGLES11::SetRotation(float angle)
{
	//m_Rotation = angle;
    Message(LOCAL_MSG, "Deprecated!");
}

void QuadBatchGLES11::SetColor(float r, float g, float b, float a)
{
	m_CurrentColors[0].Set(r, g, b, a);
	m_CurrentColors[1].Set(r, g, b, a);
	m_CurrentColors[2].Set(r, g, b, a);
	m_CurrentColors[3].Set(r, g, b, a);
}

void QuadBatchGLES11::SetColor(Vec4f* c)
{
	SetColor(c->r, c->g, c->b, c->a);
}

void QuadBatchGLES11::SetTextureSubset(float tl_u, float tl_v, float br_u, float br_v)
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

void QuadBatchGLES11::RotatePoint(const Math::Vec3f &center, Math::Vec3f &point, float rotation)
{
	Math::Vec3f p = point - center;
	point.x = p.x * cos(rotation) - p.y * sin(rotation) + center.x;
	point.y = p.x * sin(rotation) + p.y * cos(rotation) + center.y;
}

void QuadBatchGLES11::Rotate(float angle)
{
    Math::Mat4 t_rotmatrix = Math::Mat4::Identity;
    t_rotmatrix.Rotate(angle, 0.0f, 0.0f, 1.0f);
    m_TransformMatrix = m_TransformMatrix * t_rotmatrix;
}

void QuadBatchGLES11::Translate(float x, float y)
{
    /*m_Translation.x += x;
    m_Translation.y += y;*/
    //m_TransformMatrix = m_TransformMatrix.Translate(x, y, 0.0f);
    Math::Mat4 t_translatematrix = Math::Mat4::Identity;
    t_translatematrix.Translate(x, y, 0.0f);
        
    //m_TransformMatrix.Translate(x, y, 0.0f);
    m_TransformMatrix = m_TransformMatrix * t_translatematrix;
}

void QuadBatchGLES11::LoadIdentity()
{
    m_TransformMatrix = Math::Mat4::Identity;
}

void QuadBatchGLES11::SetDepth(float d)
{
	m_CurrentDepthLayer = d;
}

void QuadBatchGLES11::Reset()
{
	m_VertexBufferPos = 0;
	//SetRotation(0.f);
	SetTextureSubset(0.f,0.f,1.f,1.f);
	SetColor(1.f,1.f,1.f,1.f);
	
    LoadIdentity();
}

void QuadBatchGLES11::AddFreeform(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3)
{
	if (m_VertexBufferPos >= m_VertexBufferSize)
	{
		Message(LOCAL_MSG, "Not enough space in vertex array! Try increasing _maxSize in QuadBatch constructor.");
		return;
	}
	
	/*
	
	 0       1, 4
	   +----+
	   |   /|
	   |  / |
	   | /  |
	   |/   |
	   +----+  
	2, 3      5
	 
	*/
	
	// Transform into world coords
	Math::Vec4f t_pos(0.0f, 0.0f, 0.0f, 1.0f);
	
    t_pos = Math::Vec4f(x0, y0, 0.0f, 1.0f);
    t_pos = m_TransformMatrix * t_pos;
    x0 = t_pos.x;
    y0 = t_pos.y;
    
    t_pos = Math::Vec4f(x1, y1, 0.0f, 1.0f);
    t_pos = m_TransformMatrix * t_pos;
    x1 = t_pos.x;
    y1 = t_pos.y;
    
    t_pos = Math::Vec4f(x2, y2, 0.0f, 1.0f);
    t_pos = m_TransformMatrix * t_pos;
    x2 = t_pos.x;
    y2 = t_pos.y;
    
    t_pos = Math::Vec4f(x3, y3, 0.0f, 1.0f);
    t_pos = m_TransformMatrix * t_pos;
    x3 = t_pos.x;
    y3 = t_pos.y;

	// 0
	m_Vertices[m_VertexBufferPos].pos.x = x0;
	m_Vertices[m_VertexBufferPos].pos.y = y0;
    m_Vertices[m_VertexBufferPos].pos.z = m_CurrentDepthLayer;
	m_Vertices[m_VertexBufferPos].tex = m_CurrentTexCoords[0];
	m_Vertices[m_VertexBufferPos].color = m_CurrentColors[0];
	//RotatePoint(center, m_Vertices[m_VertexBufferPos].pos);
	
	// 1
	m_Vertices[m_VertexBufferPos+1].pos.x = x1;
	m_Vertices[m_VertexBufferPos+1].pos.y = y1;
	m_Vertices[m_VertexBufferPos+1].pos.z = m_CurrentDepthLayer;
	m_Vertices[m_VertexBufferPos+1].tex = m_CurrentTexCoords[1];
	m_Vertices[m_VertexBufferPos+1].color = m_CurrentColors[1];
	//RotatePoint(center, m_Vertices[m_VertexBufferPos+1].pos);
	
	// 2
	m_Vertices[m_VertexBufferPos+2].pos.x = x3;
	m_Vertices[m_VertexBufferPos+2].pos.y = y3;
	m_Vertices[m_VertexBufferPos+2].pos.z = m_CurrentDepthLayer;
	m_Vertices[m_VertexBufferPos+2].tex = m_CurrentTexCoords[3];
	m_Vertices[m_VertexBufferPos+2].color = m_CurrentColors[3];
	//RotatePoint(center, m_Vertices[m_VertexBufferPos+2].pos);
	
	// 3
	m_Vertices[m_VertexBufferPos+3].pos.x = x3;
	m_Vertices[m_VertexBufferPos+3].pos.y = y3;
	m_Vertices[m_VertexBufferPos+3].pos.z = m_CurrentDepthLayer;
	m_Vertices[m_VertexBufferPos+3].tex = m_CurrentTexCoords[3];
	m_Vertices[m_VertexBufferPos+3].color = m_CurrentColors[3];
	//RotatePoint(center, m_Vertices[m_VertexBufferPos+3].pos);
	
	// 4
	m_Vertices[m_VertexBufferPos+4].pos.x = x1;
	m_Vertices[m_VertexBufferPos+4].pos.y = y1;
	m_Vertices[m_VertexBufferPos+4].pos.z = m_CurrentDepthLayer;
	m_Vertices[m_VertexBufferPos+4].tex = m_CurrentTexCoords[1];
	m_Vertices[m_VertexBufferPos+4].color = m_CurrentColors[1];
	//RotatePoint(center, m_Vertices[m_VertexBufferPos+4].pos);
	
	// 5
	m_Vertices[m_VertexBufferPos+5].pos.x = x2;
	m_Vertices[m_VertexBufferPos+5].pos.y = y2;
	m_Vertices[m_VertexBufferPos+5].pos.z = m_CurrentDepthLayer;
	m_Vertices[m_VertexBufferPos+5].tex = m_CurrentTexCoords[2];
	m_Vertices[m_VertexBufferPos+5].color = m_CurrentColors[2];
	//RotatePoint(center, m_Vertices[m_VertexBufferPos+5].pos);
	
	m_VertexBufferPos += 6;
}

void QuadBatchGLES11::AddTopLeft(float x, float y, float w, float h)
{
	if (m_VertexBufferPos >= m_VertexBufferSize)
	{
		Message(LOCAL_MSG, "Not enough space in vertex array! Try increasing _maxSize in QuadBatch constructor.");
		return;
	}
	
    AddFreeform(x, y, x+w, y, x+w, y+h, x, y+h);
	

	/*Math::Vec3f center(x + w/2, y + h/2, 0.f);
    
    Math::Vec4f t_pos(x, y, 0.0f, 1.0f);
    t_pos = m_TransformMatrix * t_pos;
    x = t_pos.x;
    y = t_pos.y;
	
	// 0
	m_Vertices[m_VertexBufferPos].pos.x = x;
	m_Vertices[m_VertexBufferPos].pos.y = y;
	m_Vertices[m_VertexBufferPos].tex = m_CurrentTexCoords[0];
	m_Vertices[m_VertexBufferPos].color = m_CurrentColors[0];
	RotatePoint(center, m_Vertices[m_VertexBufferPos].pos);
	
	// 1
	m_Vertices[m_VertexBufferPos+1].pos.x = x+w;
	m_Vertices[m_VertexBufferPos+1].pos.y = y;
	m_Vertices[m_VertexBufferPos+1].tex = m_CurrentTexCoords[1];
	m_Vertices[m_VertexBufferPos+1].color = m_CurrentColors[1];
	RotatePoint(center, m_Vertices[m_VertexBufferPos+1].pos);
	
	// 2
	m_Vertices[m_VertexBufferPos+2].pos.x = x;
	m_Vertices[m_VertexBufferPos+2].pos.y = y+h;
	m_Vertices[m_VertexBufferPos+2].tex = m_CurrentTexCoords[3];
	m_Vertices[m_VertexBufferPos+2].color = m_CurrentColors[3];
	RotatePoint(center, m_Vertices[m_VertexBufferPos+2].pos);
	
	// 3
	m_Vertices[m_VertexBufferPos+3].pos.x = x;
	m_Vertices[m_VertexBufferPos+3].pos.y = y+h;
	m_Vertices[m_VertexBufferPos+3].tex = m_CurrentTexCoords[3];
	m_Vertices[m_VertexBufferPos+3].color = m_CurrentColors[3];
	RotatePoint(center, m_Vertices[m_VertexBufferPos+3].pos);
	
	// 4
	m_Vertices[m_VertexBufferPos+4].pos.x = x+w;
	m_Vertices[m_VertexBufferPos+4].pos.y = y;
	m_Vertices[m_VertexBufferPos+4].tex = m_CurrentTexCoords[1];
	m_Vertices[m_VertexBufferPos+4].color = m_CurrentColors[1];
	RotatePoint(center, m_Vertices[m_VertexBufferPos+4].pos);
	
	// 5
	m_Vertices[m_VertexBufferPos+5].pos.x = x+w;
	m_Vertices[m_VertexBufferPos+5].pos.y = y+h;
	m_Vertices[m_VertexBufferPos+5].tex = m_CurrentTexCoords[2];
	m_Vertices[m_VertexBufferPos+5].color = m_CurrentColors[2];
	RotatePoint(center, m_Vertices[m_VertexBufferPos+5].pos);

	m_VertexBufferPos += 6;*/
}

void QuadBatchGLES11::AddCentered(float x, float y, float w, float h)
{
	AddTopLeft(x-w/2.0f, y-h/2.0f, w, h);
}

void QuadBatchGLES11::AddCentered(float x, float y, float w, float h, float rotation)
{
    Math::Vec3f center(x, y, 0.0f);
    Math::Vec3f p0, p1, p2, p3;
    
    p0.x = x-w/2.0f;
    p1.x = p0.x + w;
    p2.x = p1.x;
    p3.x = p0.x;
    
    p0.y = y-h/2.0f;
    p1.y = p0.y;
    p2.y = p0.y + h;
    p3.y = p2.y;
    
    RotatePoint(center, p0, rotation);
    RotatePoint(center, p1, rotation);
    RotatePoint(center, p2, rotation);
    RotatePoint(center, p3, rotation);
    
    AddFreeform(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
}

void QuadBatchGLES11::Draw()
{
	Flush();
}

void QuadBatchGLES11::Flush()
{
	if (m_VertexBufferPos == 0)
		return;

	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (char*)m_Vertices);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (char*)m_Vertices + sizeof(float) * 3);
	glColorPointer(4, GL_FLOAT, sizeof(Vertex), (char*)m_Vertices + sizeof(float) * 5);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glDrawArrays(GL_TRIANGLES, 0, m_VertexBufferPos);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}
