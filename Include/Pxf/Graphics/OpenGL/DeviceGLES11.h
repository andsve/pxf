#ifndef _PXF_GRAPHICS_DEVICEGLES11_H_
#define _PXF_GRAPHICS_DEVICEGLES11_H_

#include <Pxf/Graphics/Device.h>
#include <Pxf/Graphics/OpenGL/VideoBufferGL.h>
#import <OpenGLES/ES1/glext.h>

namespace Pxf{
	namespace Graphics {
		class RenderTarget;
		class VertexBuffer;
		class WindowSpecifications;
		class Window;
		class Texture;

		class DeviceGLES11 : public Device
		{
		public:
			DeviceGLES11();
			~DeviceGLES11();
			
			Window* OpenWindow(WindowSpecifications* _pWindowSpecs);
			void CloseWindow();
			DeviceType GetDeviceType() { return EOpenGLES11; }
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

			VideoBuffer* CreateVideoBuffer(int _Format = GL_RENDERBUFFER_OES, int _Width = 0, int _Height = 0);
			void DeleteVideoBuffer(VideoBuffer* _VideoBuffer);
			bool BindVideoBuffer(VideoBuffer* _VideoBuffer);
			
			// TODO: Remove and put this in createvideobuffer
			//VideoBuffer* CreateFrameBuffer();
			
		private:
			/*
			GLuint m_FrameBuffer;
			GLuint m_RenderBuffer;
			GLuint m_DepthBuffer;
			
		#ifdef __OBJC__
			EAGLContext*			m_Context;

		#endif */

		};
	} // Graphics
} // Pxf

#endif //_PXF_GRAPHICS_DEVICEGLES1_H_
