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
	if(m_D3D9)
	{
		m_D3D9->Release();
		m_D3D9 = NULL;
	}
	if(m_D3D9_device)
	{
		//??
		//m_D3D9_device->
	}
}

Window* DeviceD3D9::OpenWindow(WindowSpecifications* _pWindowSpecs)
{
	WindowD3D* t_wnd = new WindowD3D(_pWindowSpecs);

	if(!t_wnd)
		Message(LOCAL_MSG,"Could not create Window");
	else
	{
		t_wnd->InitWindow();
		t_wnd->InitD3D();
		m_D3D9_device = t_wnd->GetD3DDevice();
	}

	return t_wnd;
}

void DeviceD3D9::CloseWindow(Window* _pWindow)
{

}
VertexBuffer* DeviceD3D9::CreateVertexBuffer()
{
	return NULL;
}
void DeviceD3D9::DestroyVertexBuffer(VertexBuffer* _pVertexBuffer)
{

}
void DeviceD3D9::DrawVertexBuffer(VertexBuffer* _pVertexBuffer)
{

}

#endif // CONF_FAMILY_WINDOWS