#ifndef __PXF_RESOURCE_SHADER_H__
#define __PXF_RESOURCE_SHADER_H__

#include <Pxf/Base/Debug.h>

namespace Pxf {
	namespace Resource {
		class DeviceResource;
	}

	namespace Graphics {
		
		class Shader : Resource::DeviceResource
		{
		private:
			static unsigned _ID;
			bool m_IsValid;
			bool Compile();

		public:
			Shader() : Resource::DeviceResource(..)
			{

			}

			virtual void Bind() = 0;
			virtual void Unbind() = 0;
			
			inline bool IsValid()
			{
				return m_IsValid;
			}
		};
	}
}


#endif // __PXF_RESOURCE_SHADER_H__
