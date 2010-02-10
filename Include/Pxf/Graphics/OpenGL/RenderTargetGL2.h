#ifndef _PXF_GRAPHICS_RENDERTARGETGL2_H_
#define _PXF_GRAPHICS_RENDERTARGETGL2_H_

#include <Pxf/Graphics/RenderTarget.h>
#include <Pxf/Graphics/OpenGL/OpenGL.h>

namespace Pxf
{
	namespace Graphics
	{			
		const GLenum FBO_Buffers[] = { 	GL_COLOR_ATTACHMENT0_EXT, 
										GL_COLOR_ATTACHMENT1_EXT,
										GL_COLOR_ATTACHMENT2_EXT,
										GL_COLOR_ATTACHMENT3_EXT };

		class Texture;

		class FBO : public RenderTarget
		{
		private:
			bool 	m_MRTEnabled;
			
			int 	m_Width;
			int 	m_Height;

			// balls, just realized color formats already have been set by the device when the textures are created.. 
			// to maintain "FBO completeness", the same color formats must be used in all color attachments
			GLenum	m_ColorFormat;
			GLenum 	m_DepthFormat;
			GLuint 	m_FBOHandle;

			// pointers instead?
			GLuint 	m_ColorAttachments[4];
			GLuint 	m_DepthAttachment;

			void _Initialize();
		public:
			FBO() : RenderTarget(RT_TYPE_FBO)
			{
				_Initialize();
			}

			~FBO();
			void AddColorAttachment(Texture* _Color);
			void AddDepthAttachment(Texture* _Depth);

			GLuint GetFBOHandle() { return m_FBOHandle; }
			GLuint* GetColorAttachments() { return m_ColorAttachments; }
		};

		class PBO : public RenderTarget
		{
		private:
		public:
			PBO() : RenderTarget(RT_TYPE_PBO)
			{
				
			}
			~PBO();
			
			void AddColorAttachment(Texture* _Color);
			void AddDepthAttachment(Texture* _Depth);
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_RENDERTARGETGL2_H_