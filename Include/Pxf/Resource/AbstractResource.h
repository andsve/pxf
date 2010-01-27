#ifndef __PXF_RESOURCE_ABSTRACTRESOURCE_H__
#define __PXF_RESOURCE_ABSTRACTRESOURCE_H__

#include <Pxf/Base/Types.h>
#include <Pxf/Util/Noncopyable.h>
#include <Pxf/Util/String.h>

namespace Pxf {
namespace Resource {

	class Chunk;

	class AbstractResource : public Util::Noncopyable
	{
	protected:
		Chunk* m_Chunk;
		Util::String m_Source;
		unsigned m_References;
		friend class ResourceManager;

		virtual bool Build() pure;

	public:
		AbstractResource(Chunk* _Chunk, const char* _Source)
			: m_Chunk(_Chunk)
			, m_References(0)
			, m_Source(_Source)
		{}
		virtual ~AbstractResource()
		{
			// TODO: Move deletion of chunk here?
			// SafeDelete(m_Chunk); */
		};

		virtual const bool IsReady() const pure;

		Util::String GetSource() const
		{
			return m_Source;
		}
	};

} // Resource
} // Pxf

#endif //__PXF_RESOURCE_ABSTRACTRESOURCE_H__


