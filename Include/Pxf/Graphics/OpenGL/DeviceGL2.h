#ifndef _PXF_GRAPHICS_DEVICEGL2_H_
#define _PXF_GRAPHICS_DEVICEGL2_H_

#include <Pxf/Graphics/OpenGL/OpenGL.h>
#include <Pxf/Graphics/Device.h>
#include <Pxf/Graphics/Window.h>
#include <Pxf/Graphics/OpenGL/WindowGL2.h>

namespace Pxf{
	namespace Graphics {

		class QuadBatch;
		class VertexBuffer;
		class RenderTarget;

		class DeviceGL2 : public Device
		{
		public:
			DeviceGL2();
			virtual ~DeviceGL2();

			Window* OpenWindow(WindowSpecifications* _pWindowSpecs);
			void CloseWindow();

			DeviceType GetDeviceType() { return EOpenGL2; }

            void GetSize(int *_w, int *_h);
			void SetViewport(int _x, int _y, int _w, int _h);
			void SetProjection(Math::Mat4 *_matrix);
			void SwapBuffers();
			void Translate(Math::Vec3f _translate);

			Texture* CreateEmptyTexture(int _Width,int _Height, TextureFormatStorage _Format = TEX_FORMAT_RGBA);
			Texture* CreateTexture(const char* _filepath);
			Texture* CreateTexture(const char* _filepath, bool _autoload);
			Texture* CreateTextureFromData(const unsigned char* _datachunk, int _width, int _height, int _channels);
			void BindTexture(Texture* _texture);
			void BindTexture(Texture* _texture, unsigned int _texture_unit);

			QuadBatch* CreateQuadBatch(int _maxSize);

			VertexBuffer* CreateVertexBuffer(VertexBufferLocation _VertexBufferLocation, VertexBufferUsageFlag _VertexBufferUsageFlag);
			void DestroyVertexBuffer(VertexBuffer* _pVertexBuffer);
			void DrawBuffer(VertexBuffer* _pVertexBuffer);

			void BindRenderTarget(RenderTarget* _RenderTarget);
			void BindRenderTarget(RenderTarget* _RenderTarget, int _DrawID);
			void ReleaseRenderTarget(RenderTarget* _RenderTarget);
			RenderTarget* CreateRenderTarget(int _Width,int _Height,RTFormat _ColorFormat,RTFormat _DepthFormat);
		private:
			Window* m_Window;
		};

	} // Graphics
} // Pxf

#endif //_PXF_GRAPHICS_DEVICEGL2_H_
