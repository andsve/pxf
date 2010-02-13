#ifndef __PXF_RESOURCE_GLSLSHADER_H__
#define __PXF_RESOURCE_GLSLSHADER_H__

#include <Pxf/Resource/ShaderSource.h>
#include <Pxf/Resource/Chunk.h>
#include <Pxf/Base/Debug.h>

#define LOCAL_MSG "GLSLShader"

namespace Pxf {
	namespace Graphics {

		// TODO: redo.
		class GLSLComponent : public Resource::ShaderSource
		{
		private:
			unsigned m_ShaderHandle;
			bool m_IsValid;
			bool m_IsAttached;
			bool m_SourceChanged;
			void _Init();
		public:
			GLSLComponent(Resource::Chunk* _Chunk, const char* _Source,Resource::SHType _Type) 
				: Resource::ShaderSource(_Chunk,_Source,_Type)
				,m_ShaderHandle(0)
			{
				_Init();	
			}

			~GLSLComponent();
		
			unsigned GetHandle() const { return m_ShaderHandle; }
			bool IsValid() { return m_IsValid; }
			void Attach(unsigned _Handle);
			void Detach(unsigned _Handle);
			bool Reload();
			bool Load();
		};

		class GLSLShader
		{
		private:
			unsigned m_ProgramHandle;
			GLSLComponent* m_VertexProgram;
			GLSLComponent* m_FragmentProgram;
	
			static unsigned int _ID; 
			bool m_IsValid;
			bool m_IsBound;
			void _Init();			
		public:
			GLSLShader(GLSLComponent* _Vertex,GLSLComponent* _Fragment)
			{
				// simple type checking
				if(_Vertex->GetType() == Resource::SH_TYPE_VERTEX)
					m_VertexProgram = _Vertex;
				else
					Message(LOCAL_MSG,"Vertex program has wrong type");	
				if(_Fragment->GetType() == Resource::SH_TYPE_FRAGMENT)
					m_FragmentProgram = _Fragment;
				else
					Message(LOCAL_MSG,"Fragment progam has wrong type");
				_Init();
			}
			~GLSLShader();

			void Bind();
			void Unbind();
			bool IsValid() { return m_IsValid; }
			unsigned GetHandle() const { return m_ProgramHandle; }
			Pxf::Util::String GetString();
		};
	}
}


#endif // __PXF_RESOURCE_SHADER_H__
