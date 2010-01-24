#include <Pxf/Pxf.h>
#include <Pxf/Util/String.h>
#include <Pxf/Graphics/QuadBatch.h>
#include <Pxf/Graphics/OpenGL/VertexBufferGL.h>
#include <Pxf/Graphics/OpenGL/DeviceGL2.h>
#include <Pxf/Graphics/OpenGL/QuadBatchGL2.h>
#include <Pxf/Graphics/PrimitiveType.h>
#include <Pxf/Input/OpenGL/InputGL2.h>
#include <Pxf/Base/Debug.h>

#include <Pxf/Graphics/OpenGL/OpenGL.h>

#define LOCAL_MSG "DeviceGL2"

using namespace Pxf;
using namespace Pxf::Graphics;
using Util::String;

DeviceGL2::DeviceGL2()
{
	// Initialize GLFW
	if (glfwInit() != GL_TRUE)
	{
		Message(LOCAL_MSG, "Could not initialize GLFW!");
		return;
	}

	Message(LOCAL_MSG, "Device initiated.");
	
}

DeviceGL2::~DeviceGL2()
{

	// Close any open window.
	CloseWindow();
	

	// Terminate GLFW
	glfwTerminate();
}

Window* DeviceGL2::OpenWindow(WindowSpecifications* _pWindowSpecs)
{
	m_Window = new WindowGL2(_pWindowSpecs);
	m_Window->Open();
	return m_Window;
}

void DeviceGL2::CloseWindow()
{
	if (m_Window && m_Window->IsOpen())
	{
		m_Window->Close();
		delete m_Window;
	}
}

void DeviceGL2::SwapBuffers()
{
	if (m_Window)
	{
		m_Window->Swap();
	}
}


QuadBatch* DeviceGL2::CreateQuadBatch(int _maxSize)
{
	return new QuadBatchGL2(_maxSize);
}


VertexBuffer* DeviceGL2::CreateVertexBuffer(VertexBufferLocation _VertexBufferLocation, VertexBufferType _VertexBufferType)
{
	switch(_VertexBufferLocation)
	{
	//case VB_LOCATION_CPU:;
	//case VB_LOCATION_GPU: return new VertexBufferGL2_VBO();
	}
	PXFASSERT(0, "Not implemented");
	return NULL;
}

void DeviceGL2::DestroyVertexBuffer(VertexBuffer* _pVertexBuffer)
{
	PXFASSERT(0, "Not implemented");
}

static unsigned LookupPrimitiveType(PrimitiveType _PrimitiveType)
{
	switch(_PrimitiveType)
	{
		case EPointList: return GL_POINTS;
		case ELineList: return GL_LINES;
		case ELineStrip: return GL_LINE_STRIP;
		case ETriangleList: return GL_TRIANGLES;
		case ETriangleStrip: return GL_TRIANGLE_STRIP;
		case ETriangleFan: return GL_TRIANGLE_FAN;
	}
	PXFASSERT(0, "Unknown primitive type.");
	return 0;
}

void DeviceGL2::DrawVertexBuffer(VertexBuffer* _pVertexBuffer)
{
	PXFASSERT(0, "Not implemented");
}

