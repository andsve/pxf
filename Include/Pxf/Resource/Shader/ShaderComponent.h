#ifndef __PXF_RESOURCE_SHADER_H__
#define __PXF_RESOURCE_SHADER_H__

#include <Pxf/Resource/ShaderSource.h>

namespace Pxf {
	namespace Resource {
		// distinguish between different shader languages types in some way???
		enum SHType {
			SH_TYPE_VERTEX = 0,
			SH_TYPE_FRAGMENT
		};

		/*
		class ShaderProgram 
		{
		private:

		public:
			ShaderProgram(const char* _Vertex,const char* _Fragment);
			ShaderProgram(Shader* _Vertex, Shader* _Fragment);
			~ShaderProgram();
		};
		*/

		class ShaderComponent : public ShaderSource
		{
		private:
			SHType m_SType;

		public:
			ShaderComponent(Chunk* _Chunk, const char* _Source, SHType _Type) : ShaderSource(_Chunk,_Source)
			{
				m_SType = _Type;
			}
		};
	}
}


#endif // __PXF_RESOURCE_SHADER_H__
