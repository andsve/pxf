#ifndef _PXF_GRAPHICS_DEVICEGLES11_H_
#define _PXF_GRAPHICS_DEVICEGLES11_H_

#include <Pxf/Graphics/Device.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/EAGL.h>
#import <QuartzCore/QuartzCore.h>

// Special import for iPhoneInput
#import <Pxf/Extra/iPhoneInput/iPhoneInput.h>
#include <Pxf/Graphics/EAGLView.h>

namespace Pxf{
	namespace Graphics {
		class RenderTarget;
		class VertexBuffer;
		class VideoBufferGL;
		class VideoBuffer;
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
            void GetSize(int *_w, int *_h);
			void SetViewport(int _x, int _y, int _w, int _h);
			void SetProjection(Math::Mat4 *_matrix);
			void Translate(Math::Vec3f _translate);
			void SwapBuffers();

			// Texture
			Texture* CreateEmptyTexture(int _Width,int _Height,TextureFormatStorage _Format = TEX_FORMAT_RGBA);
			Texture* CreateTexture(const char* _filepath);
			Texture* CreateTexture(const char* _filepath, bool _autoload);
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
			bool UnBindVideoBufferType(int _FormatType);
			
			void SetEAGLContext(EAGLContext* _Context) { m_Context = _Context; }
			void SetUseDepthBuffer(bool _Toggle) { m_UseDepthBuffer = _Toggle; }
			void SetBackingWidth(GLint _Width) { m_BackingWidth = _Width; }
			void SetBackingHeight(GLint _Height) { m_BackingHeight = _Height; }
			GLint GetBackingWidth() { return m_BackingWidth; }
			GLint GetBackingHeight() { return m_BackingHeight; }
			
			//bool InitBuffers();
            bool InitBuffers(EAGLContext* _context, CAEAGLLayer* _EAGLLayer); // Temporary..
            
            // Input handling
            // This is fuuugly, but hey, it sucks to mix c++ and obj-c.
            void SetUIView(UIView* _view);
            void InitInput();
            void RequestTextInput(const char* _title, const char* _message, const char* _textfield);
            void InputClearResponse();
            bool InputHasRespondedText();
            void InputGetResponseText(char *outText);
            int  InputGetResponseButton();
            
            
            bool InputTap(InputTapData* _data);
            bool InputRelease(InputTapData* _data);
            bool InputDoubleTap(InputTapData* _data);
            bool InputDrag(InputDragData* _data);
            
            void InputSetTap(float x, float y);
            void InputSetRelease(float x, float y);
            void InputSetDoubleTap(float x, float y);
            void InputSetDrag(float x1, float y1, float x2, float y2);
            
            /*
            - (void) clearResponse;
            - (bool) hasRespondedToInput;
            - (void) getInputResponseText:(const char *)outText;
            - (int) getInputResponseButton;
            */
			
			EAGLContext* GetEAGLContext() { return m_Context; }
			bool GetUseDepthBuffer() { return m_UseDepthBuffer; }
			VideoBufferGL* GetRenderBuffer() { return m_RenderBuffer; }
			VideoBufferGL* GetFrameBuffer() { return m_FrameBuffer; }
			VideoBufferGL* GetDepthBuffer() { return m_DepthBuffer; }
			
		private:
			void _ConfigureTextureUnits();
			
			// TODO: replace videobuffer with simple gluints..
			/*GLuint					m_RenderBuffer;
			 GLuint					m_FrameBuffer;
			 GLuint					m_DepthBuffer; */
            UIView* m_View;
			EAGLContext*	m_Context;
			VideoBufferGL*	m_RenderBuffer;
			VideoBufferGL*	m_FrameBuffer;
			VideoBufferGL*	m_DepthBuffer;
			
			bool	m_UseDepthBuffer;
			GLint	m_BackingWidth;
			GLint	m_BackingHeight;
			
			// Input device for iPhone
            InputHandler* m_InputHandler;
            bool m_InputTapOccurred;
            bool m_InputReleaseOccurred;
            bool m_InputDoubleTapOccurred;
            bool m_InputDragOccurred;
            float m_InputTapData[2];
            float m_InputReleaseData[2];
            float m_InputDoubleTapData[2];
            float m_InputDragData[4];
            
		};
	} // Graphics
} // Pxf

#endif //_PXF_GRAPHICS_DEVICEGLES1_H_
