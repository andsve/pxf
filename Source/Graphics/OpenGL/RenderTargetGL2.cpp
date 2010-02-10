#include <Pxf/Graphics/Opengl/RenderTargetGL2.h>
#include <Pxf/Graphics/Opengl/TextureGL2.h>

using namespace Pxf;
using namespace Pxf::Graphics;

#define LOCAL_MSG "RenderTargetGL2"

void FBO::AddColorAttachment(Texture* _Color)
{
	glEnable(GL_TEXTURE_2D);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBOHandle);

	bool t_Sucess = false;

	for(int i = 0; i < 4; i++)
	{
		if(m_ColorAttachments[i] != NULL)
		{
			m_ColorAttachments[i] = ((TextureGL2*)_Color)->GetTextureID();
			glFramebufferTexture2D(GL_FRAMEBUFFER, FBO_Buffers[i],GL_TEXTURE_2D, m_ColorAttachments[i], 0);
			t_Sucess = true;
		}
	}

	if(!t_Sucess)
		Message(LOCAL_MSG,"Could not attach color texture to FBO");
	
	glDisable(GL_TEXTURE_2D);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::AddDepthAttachment(Texture* _Depth)
{
	glEnable(GL_TEXTURE_2D);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBOHandle);

	m_DepthAttachment = ((TextureGL2*)_Depth)->GetTextureID();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D, m_DepthAttachment, 0);
	
	glDisable(GL_TEXTURE_2D);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FBO::FBO(int _Width,int _Height,GLenum _ColorFormat = GL_RGBA8, GLenum _DepthFormat = GL_DEPTH_COMPONENT)
{
	m_Width = _Width;
	m_Height = _Height;
	m_ColorFormat = _ColorFormat;
	m_DepthFormat = _DepthFormat;

	// generate handles
	glEnable(GL_TEXTURE_2D);
	glGenFramebuffers(1, &m_FBOHandle);
	glDisable(GL_TEXTURE_2D);
}

FBO::~FBO()
{
	//
	glDeleteFramebuffersEXT(1,&m_FBOHandle);
}

PBO::PBO()
{
	//
}

PBO::~PBO()
{
	//
}
