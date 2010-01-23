#include <Pxf/Pxf.h>
#include <Pxf/Util/String.h>
#include <Pxf/Graphics/QuadBatch.h>
#include <Pxf/Graphics/OpenGL/DeviceGL2.h>
#include <Pxf/Graphics/OpenGL/TextureGL2.h>
#include <Pxf/Graphics/OpenGL/QuadBatchGL2.h>
#include <Pxf/Input/OpenGL/InputGL2.h>
#include <Pxf/Base/Debug.h>

#include <GL/glfw.h>

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

void DeviceGL2::SetViewport(int _x, int _y, int _w, int _h)
{
	glViewport(_x, _y, _w, _h);
}

void DeviceGL2::SetProjection(Math::Mat4 *_matrix)
{
	glMatrixMode (GL_PROJECTION);
	glLoadMatrixf((GLfloat*)(_matrix->m));
	glMatrixMode (GL_MODELVIEW);
}

void DeviceGL2::SwapBuffers()
{
	if (m_Window)
	{
		m_Window->Swap();
	}
}

Texture* DeviceGL2::CreateTexture(const char* _filepath)
{
	TextureGL2* _tex = new TextureGL2();
	_tex->Load(_filepath);
	return _tex;
}

void DeviceGL2::BindTexture(Texture* _texture)
{
	glBindTexture(GL_TEXTURE_2D, ((TextureGL2*)_texture)->GetTextureID());
}

static GLuint _texture_units_array[16] = {GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE2, GL_TEXTURE3, GL_TEXTURE4,
										GL_TEXTURE5, GL_TEXTURE6, GL_TEXTURE7, GL_TEXTURE8, GL_TEXTURE9,
										GL_TEXTURE10, GL_TEXTURE11, GL_TEXTURE12, GL_TEXTURE13, GL_TEXTURE14,
										GL_TEXTURE15};
void DeviceGL2::BindTexture(Texture* _texture, unsigned int _texture_unit)
{
	glActiveTextureARB(_texture_units_array[_texture_unit]);
	glBindTexture(GL_TEXTURE_2D, ((TextureGL2*)_texture)->GetTextureID());
}


QuadBatch* DeviceGL2::CreateQuadBatch(int _maxSize)
{
	return new QuadBatchGL2(_maxSize);
}
