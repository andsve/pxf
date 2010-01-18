#include <Pxf/Pxf.h>
#ifdef CONF_FAMILY_WINDOWS
#include <Pxf/Util/String.h>
#include <Pxf/Graphics/D3D9/DeviceD3D9.h>
#include <Pxf/Base/Debug.h>

#define LOCAL_MSG "DeviceD3D9"

using namespace Pxf;
using namespace Pxf::Graphics;
using Util::String;

DeviceD3D9::DeviceD3D9()
{
	m_D3D9 = Direct3DCreate9( D3D_SDK_VERSION);
	if(!m_D3D9){
		//Handle error
		Message(LOCAL_MSG,"Could not create device context");
	}
}

DeviceD3D9::~DeviceD3D9()
{
	if(m_D3D9)
	{
		m_D3D9->Release();
		m_D3D9 = NULL;
	}
}

#endif // CONF_FAMILY_WINDOWS