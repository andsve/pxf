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
			bool m_IsValid;
			bool m_IsAttached;
			void _Init();
		public:
			GLSLComponent(Resource::Chunk* _Chunk, const char* _Source,Resource::SHType _Type) : Resource::ShaderComponent(_Chunk,_Source,_Type)
			{
				_Init();	
			}

			~GLSLComponent();
			
			//void SetAttached(bool _Val) { m_IsAttached = _Val; }
			unsigned GetHandle() const { return m_ShaderHandle; }
			bool IsValid() { return m_IsValid; }
			//bool IsAttached() { return m_IsAttached; }
			bool Reload();
			bool Load();
		};

		class GLSLShader
		{
		private:
			unsigned m_ProgramHandle;
			GLSLComponent* m_VertexProgram;
			GLSLComponent* m_FragmentProgram;
			bool m_IsValid;
			bool m_IsBound;
			void _Init();			
		public:
			GLSLShader(GLSLComponent* _Vertex,GLSLComponent* _Fragment)
			{
				_Init();
			}
			~GLSLShader();

			void Bind();
			void Unbind();
			bool IsValid() { return m_IsValid; }
			unsigned GetHandle() const { return m_ProgramHandle; }
		};
	}
}


#endif // __PXF_RESOURCE_SHADER_H__
