#ifndef __PXF_RESOURCE_CHUNK_H__
#define __PXF_RESOURCE_CHUNK_H__

#include <Pxf/Base/Types.h>
#include <Pxf/Util/Noncopyable.h>
#include <Pxf/Util/Tools.h>

namespace Pxf {
namespace Resource {

	class Chunk : public Util::Noncopyable
	{
	public:
		char* Data;
		unsigned Size;
		
		// If data isn't allocated on the heap, it should not attempt to delete it.
		bool Static;
		
		Chunk()
			: Data(NULL)
			, Size(0)
			, Static(false) 
		{}
		
		~Chunk()
		{
			if (!Static)
			{
				Util::SafeDeleteArray(Data);
				Data = NULL;
				Size = 0;
			}
		};
	};

} // Resource
} // Pxf

#endif //__PXF_RESOURCE_CHUNK_H__


