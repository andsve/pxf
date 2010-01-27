#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/Chunk.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Stream.h>
#include <Pxf/Base/Utils.h>

#include <cstring>

using namespace Pxf::Resource;
using namespace Pxf;

using Pxf::Util::Map;
using Pxf::Util::String;

ResourceManager::ResourceManager()
{
	Message("ResourceManager", "Created");
}

ResourceManager::~ResourceManager()
{
	if (m_Resources.size() > 0)
	{
		Message("ResourceManager", "Cleaning up unreleased resources");
		Map<String, AbstractResource*>::iterator iter;
		for(iter = m_Resources.begin(); iter != m_Resources.end(); ++iter)
		{
			Message("ResourceManager", "Deleting resource '%s' [refs = %d]", iter->second->m_Source.c_str(), iter->second->m_References);
			SafeDelete(iter->second);
		}
	}

	Message("ResourceManager", "Destroyed");
}

Chunk* ResourceManager::ReadFile(const char* _Filename)
{
	FileStream file;
	if (file.OpenReadBinary(_Filename))
	{
		uint size = file.GetSize();
		char* data = new char[size];

		if (!data)
		{
			Message("ResourceManager", "Could not create chunk, out of memory");
			file.Close();
			return NULL;
		}

		Chunk* chunk = new Chunk();

		int read = 0;
		if((read = file.Read(data, size)) > 0)
		{
			Message("ResourceManager", "Creating chunk (0x%x) from '%s' (read %d of %d bytes)", chunk, _Filename, read, size);
			chunk->Data = data;
			chunk->Size = size;
			file.Close();
			return chunk;
		}

		file.Close();
		SafeDeleteArray(data);
		SafeDelete(chunk);
		Message("ResourceManager", "Failed to read from '%s'", _Filename);
		return NULL;
	}
	
	Message("ResourceManager", "Failed to open '%s'", _Filename);
	return NULL;
}

void ResourceManager::Release(AbstractResource* _Resource, bool _Purge)
{
	if (_Resource)
	{
		_Resource->m_References--;

		if (_Resource->m_References <= 0 || _Purge)
		{			
			Map<String, AbstractResource*>::iterator iter = m_Resources.find(_Resource->m_Source);
			if (iter != m_Resources.end())
			{
				Message("ResourceManager", "Purging resource holding '%s' (%s)", _Resource->m_Source.c_str(), _Purge? "Forced":"No more refs");
				m_Resources.erase(iter);
			}

			SafeDelete(_Resource);
		}
		else
			Message("ResourceManager", "Releasing resource holding '%s' [refs = %d]", _Resource->m_Source.c_str(), _Resource->m_References);
	}
}

bool ResourceManager::Reload(AbstractResource* _Resource)
{
	if (!_Resource)
		return false;

	Message("ResourceHandler", "Reloading '%s'.", _Resource->m_Source.c_str());
	Chunk* new_chunk = ReadFile(_Resource->m_Source.c_str());

	if (!new_chunk)
	{
		Message("ResourceHandler", "Failed to create new chunk. Out of memory?");
		return false;
	}

	SafeDelete(_Resource->m_Chunk);
	_Resource->m_Chunk = new_chunk;

	// Rebuild resource
	bool r = _Resource->Build();

	if (r)
		Message("ResourceHandler", "Successfully reloaded resource!");
	else
		Message("ResourceHandler", "Failed to rebuild resource.");

	return r;
}
