#ifndef _PXF_GRAPHICS_TEXTUREGLES_H_
#define _PXF_GRAPHICS_TEXTUREGLES_H_

#include <Pxf/Graphics/DeviceResource.h>
#include <Pxf/Graphics/TextureDefs.h>
#include <Pxf/Math/Vector.h>

namespace Pxf
{
	namespace Graphics
	{
		
		//! Abstract texture class
		class TextureGLES : public Texture
		{
		public:
			TextureGLES(Device* _pDevice);
			~TextureGLES();
			void Load(const char* _filepath);
			void LoadData(const unsigned char* _datachunk, int _width, int _height, int _channels);
			void Unload();
			void Reload();

			int GetWidth() { return m_Width; }
			int GetHeight() { return m_Height; }
			int GetChannels() { return m_Channels; }

			void SetMagFilter(TextureFilter _Filter);
			void SetMinFilter(TextureFilter _Filter);

			void SetClampMethod(TextureClampMethod _Method);

			Math::Vec4f CreateTextureSubset(float _x1, float _y1, float _x2, float _y2);
			
			// OGL specific
			GLuint GetTextureID() { return m_TextureID; }

		private:
			GLuint 			m_TextureID;
			const char* 	m_Filepath;
			int 			m_Width;
			int 			m_Height;
			int				m_Channels;
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_TEXTURE_H_