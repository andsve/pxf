#ifndef _PXF_GRAPHICS_TEXTUREGL2_H_
#define _PXF_GRAPHICS_TEXTUREGL2_H_

#include <Pxf/Graphics/OpenGL/DeviceGL2.h>
#include <Pxf/Graphics/Texture.h>

namespace Pxf
{

	namespace Graphics
	{
		
		//! Abstract texture class
		class TextureGL2 : public Texture
		{
		public:
			TextureGL2();
			~TextureGL2();
			void Load(const char* _filepath);
			void Unload();
			void Reload();

			int GetWidth();
			int GetHeight();
			int GetChannels();
			
			// OGL specific
			GLuint GetTextureID();

		private:
			GLuint m_TextureID;
			Util::String m_Filepath;
			int m_Width, m_Height, m_Channels;
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_TEXTUREGL2_H_