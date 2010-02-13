#include <Pxf/Graphics/Shader/GLSLShader.h>
#include <Pxf/Graphics/OpenGL/OpenGL.h>
#include <Pxf/Base/Utils.h>

using namespace Pxf;
using namespace Graphics;

unsigned int GLSLShader::_ID = 0;

void GLSLShader::_Init()
{
	m_IsBound = false;
	bool _Sucess = true;
	_ID++;

	if(m_VertexProgram->GetFileName() == NULL || m_VertexProgram->GetSource().empty() || !m_VertexProgram->IsValid())
	{
		_Sucess = false;
		Message(LOCAL_MSG,"Vertex Program not valid");
	}

	if(m_FragmentProgram->GetFileName() == NULL || m_FragmentProgram->GetSource().empty() || !m_FragmentProgram->IsValid())
	{
		_Sucess = false;
		Message(LOCAL_MSG,"Fragment Program not valid");
	}

	if(_Sucess)	
		m_ProgramHandle	= glCreateProgramObjectARB();

	m_IsValid = _Sucess;

	m_VertexProgram->Attach(m_ProgramHandle);
	m_FragmentProgram->Attach(m_ProgramHandle);

	glLinkProgramARB(m_ProgramHandle);
	Message(LOCAL_MSG,"%s load OK",GetString().c_str());
}

GLSLShader::~GLSLShader()
{
	// clean up handles
	if(m_IsBound)
		Unbind();

	m_VertexProgram->Detach(m_ProgramHandle);
	m_FragmentProgram->Detach(m_ProgramHandle);
	glDeleteObjectARB(m_ProgramHandle);
}

Pxf::Util::String GLSLShader::GetString() 
{
	char _OutStr[16];
	sprintf_s(_OutStr,sizeof(_OutStr),"Shader %i", (int) _ID); 
	return Pxf::Util::String(_OutStr);
}

void GLSLShader::Unbind()
{
	if(m_IsBound)
	{
		glUseProgramObjectARB(0);
		m_IsBound = false;
	}
}

void GLSLShader::Bind()
{
	if(!m_IsBound)
	{
		glUseProgramObjectARB(m_ProgramHandle);
		m_IsBound = true;
	}
}



GLSLComponent::~GLSLComponent()
{
	glDeleteObjectARB(m_ShaderHandle);
}

void GLSLComponent::Attach(unsigned _Handle)
{
	if(m_IsValid)
		glAttachObjectARB(_Handle,m_ShaderHandle);
	else
		Message(LOCAL_MSG,"Can't attach an invalid shader %s",GetFileName());
}

void GLSLComponent::Detach(unsigned _Handle)
{
	glDetachObjectARB(_Handle,m_ShaderHandle);
}

void GLSLComponent::_Init()
{
	m_IsValid = false;
	m_IsAttached = false;	
	m_SourceChanged = false;

	if(GetFileName() == NULL)
	{
		Message(LOCAL_MSG,"Error, passed empty shader");
		return;
	}

	/*
	IsReady returns BAD_PTR :(

	if(!IsReady())
	{
		m_IsValid = false;
		Message(LOCAL_MSG,"%s is not ready",GetFileName());
		return;
	}*/

	if(GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
	{
		m_IsValid = true;

		if(GetType() == Resource::SH_TYPE_VERTEX)
		{
			m_ShaderHandle = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);

			if(Load())
				Message(LOCAL_MSG,"Vertex shader %s load OK",GetFileName());
			else
				Message(LOCAL_MSG,"Vertex shader %s load FAIL",GetFileName());
		}
		else if(GetType() == Resource::SH_TYPE_FRAGMENT)
		{
			m_ShaderHandle = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

			if(Load())
				Message(LOCAL_MSG,"Fragment shader %s load OK",GetFileName());
			else
				Message(LOCAL_MSG,"Fragment shader %s load FAIL",GetFileName());
		}
		else
			Message(LOCAL_MSG,"Shader type unrecognized");
	}
	else
	{
		Message(LOCAL_MSG,"GLSL not supported on this system");
	}
}

bool GLSLComponent::Load()
{
	const char* _Source = GetSource().c_str();
	// generate and compile shader source
	if(IsValid())
	{
		glShaderSourceARB(m_ShaderHandle, 1, &_Source,NULL);
		glCompileShaderARB(m_ShaderHandle);

		int infologLength = 0;
    	glGetObjectParameterivARB(m_ShaderHandle, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infologLength);

		return true;
	}
	else
		return false;
}




