#ifndef __PXF_RESOURCE_SHADERSOURCE_H__
#define __PXF_RESOURCE_SHADERSOURCE_H__

#include <Pxf/Resource/AbstractResource.h>

namespace Pxf {
	namespace Resource {
		class ShaderSource : public AbstractResource
		{
		private:
			char* m_ShaderSource;
			const char* m_FileName;

			virtual bool Build();

		public:

			ShaderSource(Chunk* _Chunk, const char* _Source);
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
		};
	}
}


#endif // __PXF_RESOURCE_SHADERSOURCE_H
