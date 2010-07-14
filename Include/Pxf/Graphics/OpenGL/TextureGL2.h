#ifndef _PXF_GRAPHICS_TEXTUREGL2_H_
#define _PXF_GRAPHICS_TEXTUREGL2_H_

#include <Pxf/Math/Vector.h>
#include <Pxf/Graphics/OpenGL/DeviceGL2.h>
#include <Pxf/Graphics/Texture.h>
#include <Pxf/Util/String.h>

namespace Pxf
{

	namespace Graphics
	{
		
		//! Abstract texture class
		class TextureGL2 : public Texture
		{
		public:
			TextureGL2(Device* _pDevice);
            TextureGL2(const char* _filepath, Device* _pDevice);
			~TextureGL2();
			
            void Load();
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
			GLuint m_TextureID;
			//Util::String m_Filepath;
			int m_Width, m_Height, m_Channels;
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_TEXTUREGL2_H_