#ifndef _PXF_ENGINE_H_
#define _PXF_ENGINE_H_

#include <Pxf/Base/Config.h>
#include <Pxf/Base/Types.h>
#include <Pxf/Base/ErrorType.h>
#include <Pxf/Graphics/DeviceType.h>

namespace Pxf
{
	namespace Graphics
	{
		class Device;
	}

	void SuperTest();
	
	class Engine
	{
	public:
		Graphics::Device* CreateDevice(Graphics::DeviceType _deviceType);
		void DestroyDevice(Graphics::Device* _pDevice);
	};
}

#endif //_PXF_ENGINE_H_