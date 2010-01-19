#include <Pxf/Pxf.h>
#ifdef CONF_FAMILY_WINDOWS
#include <Pxf/Util/String.h>
#include <Pxf/Graphics/D3D9/DeviceD3D9.h>
#include <Pxf/Graphics/D3D9/WindowD3D.h>
#include <Pxf/Base/Debug.h>

#define LOCAL_MSG "DeviceD3D9"

using namespace Pxf;
using namespace Pxf::Graphics;
using Util::String;

DeviceD3D9::DeviceD3D9()
{
	// not sure if the D3D9 object is needed anymore
	m_D3D9			= NULL;
	m_D3D9_device	= NULL;
	
	/*
	if(!m_D3D9){
		//Handle error
		Message(LOCAL_MSG,"Could not create device context");
	}*/
}

DeviceD3D9::~DeviceD3D9()
{
	/* Not needed, since WindowD3D->Close() does all this.
	if(m_D3D9)
	{
		m_D3D9->Release();
		m_D3D9 = NULL;
	}
	if(m_D3D9_device)
	{
		//??
		//m_D3D9_device->
	}*/

	CloseWindow();
}

Window* DeviceD3D9::OpenWindow(WindowSpecifications* _pWindowSpecs)
{
	m_Window = new WindowD3D(_pWindowSpecs);

	if(!m_Window)
		Message(LOCAL_MSG,"Could not create Window");
	else
	{
		/*
		m_Window->InitWindow();
		m_Window->InitD3D();
		*/
		if (m_Window->Open())
			m_D3D9_device = m_Window->GetD3DDevice();
	}

	return m_Window;
}

void DeviceD3D9::CloseWindow()
{
	if (m_Window)
		m_Window->Close();
}

void DeviceD3D9::SwapBuffers()
{
	if (m_Window)
	{
		m_Window->Swap();
	}
}
/*
VertexBuffer* DeviceD3D9::CreateVertexBuffer()
{
	return NULL;
}
void DeviceD3D9::DestroyVertexBuffer(VertexBuffer* _pVertexBuffer)
{

}
void DeviceD3D9::DrawVertexBuffer(VertexBuffer* _pVertexBuffer)
{

}*/

#endif // CONF_FAMILY_WINDOWS