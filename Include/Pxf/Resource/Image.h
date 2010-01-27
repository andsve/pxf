#ifndef __PXF_RESOURCE_IMAGE_H__
#define __PXF_RESOURCE_IMAGE_H__

#include <Pxf/Resource/AbstractResource.h>

namespace Pxf {
namespace Resource {

	class Image : public AbstractResource
	{
	private:
		int m_Width;
		int m_Height;
		int m_Channels;
		unsigned char* m_ImageData;

		virtual bool Build();


	public:
		Image(Chunk* _Chunk, const char* _Source);
		~Image();

		const int Width() const
		{
			return m_Width;
		} 

		const int Height() const
		{
			return m_Height;
		} 

		const int Channels() const
		{
			return m_Channels;
		} 
		
		unsigned char* Ptr() const
		{
			return m_ImageData;
		}

		virtual const bool IsReady() const
		{
			return m_ImageData != NULL;
		}
	};

} // Resource
} // Pxf

#endif //__PXF_RESOURCE_IMAGE_H__

