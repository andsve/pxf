#ifndef _PXF_GRAPHICS_DEVICE_H_
#define _PXF_GRAPHICS_DEVICE_H_

#include <Pxf/Math/Math.h>
#include <Pxf/Math/Vector.h>
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

		/**
		 * VertexBuffer location types
		 * VB_LOCATION_CPU is stored in local memory
		 * VB_LOCATION_GPU is stored in GPU memory
		 */
		enum VertexBufferLocation
		{
			VB_LOCATION_CPU,
			VB_LOCATION_GPU
		};

		/**
		 * VertexBuffer type
		 * VB_TYPE_INTERLEAVED: stores data interleaved in one continuous chunk
		 * VB_TYPE_INDEPENDENT: stores data in independent chunks
		 */
		enum VertexBufferType
		{
			VB_TYPE_INTERLEAVED,
			VB_TYPE_INDEPENDENT
		};

		/**
		 * VertexBuffer attributes
		 */
		enum VertexBufferAttributes
		{
			VB_VERTEX_DATA = 1,
			VB_NORMAL_DATA = 2,
			VB_TEXCOORD_DATA = 4,
			VB_COLOR_DATA = 8,
			VB_INDEX_DATA = 16,
			VB_EDGEFLAG_DATA = 32,
			VB_ATTRIB_DATA = 64
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
			virtual void SwapBuffers() = 0;

			// PrimitiveBatch
			virtual QuadBatch* CreateQuadBatch(int _maxSize) = 0;
			
			virtual VertexBuffer* CreateVertexBuffer(VertexBufferLocation _VertexBufferLocation, VertexBufferType _VertexBufferType) = 0;
			virtual void DestroyVertexBuffer(VertexBuffer* _pVertexBuffer) = 0;
			virtual void DrawVertexBuffer(VertexBuffer* _pVertexBuffer) = 0;

		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_DEVICE_H_