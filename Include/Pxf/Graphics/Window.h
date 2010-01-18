#ifndef _PXF_GRAPHICS_WINDOW_H_
#define _PXF_GRAPHICS_WINDOW_H_

namespace Pxf{
	namespace Graphics {

		class Window
		{
		public:

			virtual bool Open() = 0;
			virtual bool Close() = 0;
			virtual void Swap() = 0;

			virtual void SetTitle(const char *_title) = 0;

			virtual int GetFPS() = 0;
			virtual int GetWidth() = 0;
			virtual int GetHeight() = 0;
			virtual float GetAspectRatio() = 0;
			virtual char* GetContextTypeName() = 0;

			virtual bool IsOpen() = 0;
			/*
			// TODO: Implement IsActive and IsMinimized for WindowD3D.
			virtual bool IsActive() = 0;
			virtual bool IsMinimized() = 0;
			*/
		};

	} // Graphics
} // Pxf

#endif //_PXF_GRAPHICS_WINDOW_H_
