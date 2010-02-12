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

	if(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT)
		Message(LOCAL_MSG,"Framebuffer not ready");
}

void FBO::AddDepthAttachment(Texture* _Depth)
{
	glEnable(GL_TEXTURE_2D);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBOHandle);

	m_DepthAttachment = ((TextureGL2*)_Depth)->GetTextureID();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D, m_DepthAttachment, 0);
	
	glDisable(GL_TEXTURE_2D);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT)
		Message(LOCAL_MSG,"Framebuffer not ready");
}

void FBO::_Initialize()
{
	glEnable(GL_TEXTURE_2D);
	glGenFramebuffers(1, &m_FBOHandle);
	glDisable(GL_TEXTURE_2D);

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
}


FBO::~FBO()
{
	//
	glDeleteFramebuffersEXT(1,&m_FBOHandle);
}

PBO::~PBO()
{
	//
}

void PBO::AddColorAttachment(Texture* _Color)
{
	// 
}

void PBO::AddDepthAttachment(Texture* _Depth)
{
	Message("RenderTarget","Depth texture attachment not supported on PBOs");
}
					
