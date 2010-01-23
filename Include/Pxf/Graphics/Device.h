#ifndef _PXF_GRAPHICS_DEVICE_H_
#define _PXF_GRAPHICS_DEVICE_H_

#include <Pxf/Math/Math.h>
#include <Pxf/Math/Vector.h>
#include <Pxf/Math/Matrix.h>
#include <Pxf/Graphics/DeviceType.h>

namespace Pxf
{

	namespace Graphics
	{
		class Window;
		class WindowSpecifications;
		
		class VertexBuffer;

		class QuadBatch;

		struct Vertex
		{
			Math::Vec3f pos;
			Math::Vec2f tex;
			Math::Vec4f color;
		};
		
		//! Abstract video device
		class Device
		{
		public:

			// Windowing
			virtual Window* OpenWindow(WindowSpecifications* _pWindowSpecs) = 0;
			virtual void CloseWindow() = 0;

			// Device
			virtual DeviceType GetDeviceType() = 0;

			// Graphics
			virtual void SetViewport(int _x, int _y, int _w, int _h) = 0;
			virtual void SetProjection(Math::Mat4 *_matrix) = 0;
			virtual void SwapBuffers() = 0;

			// PrimitiveBatch
			virtual QuadBatch* CreateQuadBatch(int _maxSize) = 0;
			
			/*virtual VertexBuffer* CreateVertexBuffer() = 0;
			virtual void DestroyVertexBuffer(VertexBuffer* _pVertexBuffer) = 0;
			virtual void DrawVertexBuffer(VertexBuffer* _pVertexBuffer) = 0;*/

		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_DEVICE_H_