#ifndef __PXF_RESOURCE_SHADERSOURCE_H__
#define __PXF_RESOURCE_SHADERSOURCE_H__

#include <Pxf/Resource/AbstractResource.h>

namespace Pxf {
	namespace Resource {

		enum SHType {
			SH_TYPE_VERTEX = 0,
			SH_TYPE_FRAGMENT,
			SH_TYPE_NONE
		};

		class ShaderSource : public AbstractResource
		{
		private:
			char* m_ShaderSource;
			const char* m_FileName;
			SHType m_SType;

			virtual bool Build();

		public:

			ShaderSource(Chunk* _Chunk, const char* _Source,SHType _Type = SH_TYPE_NONE);
			~ShaderSource();

			const bool IsReady() const
			{
				return m_ShaderSource != NULL;
			}

			char* GetShaderSource() const
			{
				return m_ShaderSource;
			}

			const char* GetFileName()
			{
				return m_FileName;
			}

			SHType GetType() 
			{ 
				return m_SType; 
			}
		};
	}
}


#endif // __PXF_RESOURCE_SHADERSOURCE_H
