#ifndef _PXF_EXTRA_SIMPLEFONT_H_
#define _PXF_EXTRA_SIMPLEFONT_H_

#include <Pxf/Graphics/Device.h>
#include <Pxf/Math/Vector.h>
#include <Pxf/Util/String.h>

#include <Pxf/Extra/SimpleFont/stb_truetype.h>

#define PXF_EXTRA_SIMPLEFONT_MAXQUAD_PER_FONT 1024

namespace Pxf
{
	namespace Graphics { class QuadBatch; class Texture; }
	
	namespace Extra
	{
		class SimpleFont
		{
		public:
			SimpleFont (Util::String _font_filepath, Graphics::Device *_device);
			~SimpleFont ();
			
			void Load();
			void ResetText();
			void AddText(Util::String _text, Math::Vec3f _pos);
			void AddTextCentered(Util::String _text, Math::Vec3f _pos);
			void Draw();
		private:
			
			// Portable method to get file size
			int _get_filesize();
			
			Graphics::Device *m_Device;
			Util::String m_FontFilepath;
			Graphics::Texture *m_CharmapTexture;
			
			stbtt_bakedchar m_CharData[96];
			
			Graphics::QuadBatch *m_QuadBatch;
			
			int m_TextureSize;
			float m_FontSize;
		};
	} /* Extra */
} /* Pxf */

#endif
