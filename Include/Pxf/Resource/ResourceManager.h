#ifndef __PXF_RESOURCEMANAGER_H__
#define __PXF_RESOURCEMANAGER_H__

#include <Pxf/Util/Map.h>
#include <Pxf/Util/List.h>

#include <Pxf/Base/Debug.h>
#include <Pxf/Util/String.h>

#include <Pxf/Resource/AbstractResource.h>

namespace Pxf {
namespace Resource {

class Chunk;

class ResourceManager
{
private:
	Util::Map<Util::String, AbstractResource*> m_Resources;
	static Chunk* ReadFile(const char* _Filename);

	friend class AbstractResource;
public:
	ResourceManager();
	~ResourceManager();

	/*
		Section 9.3 of the C++-standard says this should be inlined! (but it isn't, phew). Why?
		Is it possible to rewrite this so it doesn't generate as much code if it gets inlined?

		TODO: Should have a optional flag for forcing a load from disk, e.g if the
		      file is updated and the resource is reloaded.
	*/
	template <class T>
	T* Acquire(const char* _Filename)
	{
		Util::Map<Util::String, AbstractResource*>::iterator iter = m_Resources.find(_Filename);
		AbstractResource* res = NULL;

		if(iter == m_Resources.end())
		{
			Chunk* chunk = NULL;
			chunk = ReadFile(_Filename);
			if (chunk)
			{
				res = new T(chunk, _Filename);
				if (res)
					m_Resources.insert(std::make_pair(_Filename, res));
			}
		}
		else
		{
			res = iter->second;
		}

		if (!res)
		{
			Message("ResourceManager", "Failed to acquire resource.");
			return NULL;
		}
		
		res->m_References++;
		Message("ResourceManager", "Acquired '%s' [refs = %d]", _Filename, res->m_References);
		return (T*)res;
	}

	// If purge is true; it ignores references and just deletes the resource
	void Release(AbstractResource* _Resource, bool _Purge = false);

	// Reload resource
	bool Reload(AbstractResource* _Resource);
};

} // Resource
} // Pxf

#endif //__PXF_RESOURCEMANAGER_H__


