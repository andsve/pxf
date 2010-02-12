#ifndef __PXF_RESOURCE_GLSLSHADER_H__
#define __PXF_RESOURCE_GLSLSHADER_H__

#include <Pxf/Resource/Shader/ShaderComponent.h>
#include <Pxf/Resource/Chunk.h>

namespace Pxf {
	namespace Graphics {

		class GLSLComponent : public Resource::ShaderComponent
		{
		private:
			unsigned m_ShaderHandle;
			void _Init();
		public:
			GLSLComponent(Resource::Chunk* _Chunk, const char* _Source,Resource::SHType _Type) : Resource::ShaderComponent(_Chunk,_Source,_Type)
			{
				_Init();	
			}

			~GLSLComponent();

			unsigned GetHandle() const { return m_ShaderHandle; }
			bool Reload();
			bool Load();
		};

		class GLSLShader
		{
		private:
			unsigned m_ProgramHandle;
			GLSLComponent* m_VertexProgram;
			GLSLComponent* m_FragmentProgram;
			bool m_IsBound;
			
		public:
			GLSLShader(const char* _Vertex,const char* _Fragment);
			~GLSLShader();

			void Bind();
			void Unbind();
			unsigned GetHandle() const { return m_ProgramHandle; }
		};
	}
}


#endif // __PXF_RESOURCE_SHADER_H__
