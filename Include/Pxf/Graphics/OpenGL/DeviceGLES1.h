#ifndef _PXF_GRAPHICS_DEVICEGLES1_H_
#define _PXF_GRAPHICS_DEVICEGLES1_H_

#include <Pxf/Graphics/OpenGL/OpenGL.h>
#include <Pxf/Graphics/Device.h>
#include <Pxf/Graphics/Window.h>
#include <Pxf/Graphics/OpenGL/WindowGL2.h>

namespace Pxf{
	namespace Graphics {
		class RenderTarget;
		class VertexBuffer;

		class DeviceGLES1 : public Device
		{
		public:
			DeviceType GetDeviceType() { return EOpenGLES1; }
			// Graphics
			void SetViewport(int _x, int _y, int _w, int _h);
			void SetProjection(Math::Mat4 *_matrix);
			void Translate(Math::Vec3f _translate);
			void SwapBuffers();

			// Texture
			Texture* CreateEmptyTexture(int _Width,int _Height,TextureFormatStorage _Format = TEX_FORMAT_RGBA);
			Texture* CreateTexture(const char* _filepath);
			Texture* CreateTextureFromData(const unsigned char* _datachunk, int _width, int _height, int _channels);
			void BindTexture(Texture* _texture);
			void BindTexture(Texture* _texture, unsigned int _texture_unit); // Multi-texturing

			// PrimitiveBatch
			QuadBatch* CreateQuadBatch(int _maxSize);
			
			VertexBuffer* CreateVertexBuffer(VertexBufferLocation _VertexBufferLocation, VertexBufferUsageFlag _VertexBufferUsageFlag);
			void DestroyVertexBuffer(VertexBuffer* _pVertexBuffer);
			void DrawBuffer(VertexBuffer* _pVertexBuffer);

			void BindRenderTarget(RenderTarget* _RenderTarget);
			void ReleaseRenderTarget(RenderTarget* _RenderTarget);
			RenderTarget* CreateRenderTarget(int _Width,int _Height,RTFormat _ColorFormat,RTFormat _DepthFormat);
		private:

		#ifdef __OBJC__

		#endif

		};
	} // Graphics
} // Pxf

#endif //_PXF_GRAPHICS_DEVICEGLES1_H_
