#ifndef _PXF_GRAPHICS_TEXTUREGLES_H_
#define _PXF_GRAPHICS_TEXTUREGLES_H_

#include <Pxf/Graphics/DeviceResource.h>
#include <Pxf/Graphics/TextureDefs.h>
#include <Pxf/Graphics/Texture.h>
#include <OpenGLES/ES1/gl.h>
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

			inline int GetWidth() { return m_Width; }
			inline int GetHeight() { return m_Height; }
			inline int GetChannels() { return m_Channels; }

			void SetMagFilter(TextureFilter _Filter);
			void SetMinFilter(TextureFilter _Filter);

			void SetClampMethod(TextureClampMethod _Method);

			Math::Vec4f CreateTextureSubset(float _x1, float _y1, float _x2, float _y2);
			
			// OGL specific
			inline GLuint GetTextureID() { return m_TextureID; }

		private:
		private:
			GLuint m_TextureID;
			Util::String m_Filepath;
			int m_Width, m_Height, m_Channels;
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_TEXTURE_H_