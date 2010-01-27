#ifndef __PXF_BASE_STREAM_H__
#define __PXF_BASE_STREAM_H__

#include <Pxf/Base/Types.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>

namespace Pxf {

class Stream
{
public:
	virtual uint Read(void* _Buffer, uint _Size)
	{
		(void)_Buffer;
		(void)_Size;

		PXFASSERT(0, "Stream::Read: Not implemented");
		return 0;
	}
	virtual uint Write(const void* _Buffer, uint _Size)
	{
		(void)_Buffer;
		(void)_Size;

		PXFASSERT(0, "Stream::Write: Not implemented");
		return 0;
	}

	template<typename T> T ReadNE() { T v; Read(&v, sizeof(T)); return v;}
	template<typename T> T ReadBE() { return ByteswapBtoN(ReadNE<T>()); }
	template<typename T> T ReadLE() { return ByteswapLtoN(ReadNE<T>()); }

	template<typename T> void WriteNE(T v) { Write(&v, sizeof(T)); }
	template<typename T> void WriteBE(T v) { T t = ByteswapNtoB(v); Write(&t, sizeof(T)); }
	template<typename T> void WriteLE(T v) { T t = ByteswapNtoL(v); Write(&t, sizeof(T)); }
	
};

class FileStream : public Stream
{
private:
	void* m_Handle;
	uint m_Offset;
	uint m_Remaining;
	uint m_Size;
public:
	FileStream()
		: m_Handle(NULL)
		, m_Offset(0)
		, m_Remaining(0)
		, m_Size(0)
	{}

	FileStream(const char* _FileName, const char* _Mode)
		: m_Handle(NULL)
		, m_Offset(0)
		, m_Remaining(0)
		, m_Size(0)
	{
		Open(_FileName, _Mode);
	}

	~FileStream()
	{
		if (IsValid())
			Close();
	}

	bool Open(const char* _FileName, const char *_Mode);

	bool OpenReadBinary(const char* _FileName)
	{
		return Open(_FileName, "rb");
	}

	bool OpenWriteBinary(const char* _FileName)
	{
		return Open(_FileName, "wb");
	}

	bool OpenAppendBinary(const char* _FileName)
	{
		return Open(_FileName, "ab");
	}

	bool Close();
	
	uint Read(void* _Buffer, uint _Count);
	uint Write(const void* _Buffer, uint _Count);

	bool Flush();

	bool Skip(uint _Count);
	bool SeekTo(uint _Offset);
	bool SeekToStart();
	bool SeekToEnd();

	inline bool IsValid() const
	{
		return m_Handle != NULL;
	}

	inline int GetPos() const
	{
		return m_Offset;
	}

	inline int GetRemainingBytes() const
	{
		return m_Remaining;
	}

	inline int GetSize() const
	{
		return m_Size;
	}
};


// REMEMBER Check bounds when reading and writing! Add DynamicMemoryStream?
class MemoryStream : public Stream
{
private:
	char* m_Buffer;
	uint m_Offset;
	uint m_Remaining;
	uint m_Size;
public:
	MemoryStream(char* _Buffer, uint _Size)
		: m_Buffer(_Buffer)
		, m_Size(_Size)
		, m_Remaining(_Size)
		, m_Offset(0)
	{
	}

	uint Read(void* _Buffer, uint _Count);
	uint Write(const void* _Buffer, uint _Count);

	void Skip(uint _Count);
	void SeekTo(uint _Offset);
	void SeekToStart();
	void SeekToEnd();

	inline bool IsValid() const
	{
		return m_Buffer != NULL;
	}

	inline int GetPos() const
	{
		return m_Offset;
	}

	inline int GetRemainingBytes() const
	{
		return m_Remaining;
	}

	inline int GetSize() const
	{
		return m_Size;
	}

	inline char* GetDataPointer() const
	{
		return m_Buffer;
	}
};

} // Pxf

#endif //__PXF_BASE_STREAM_H__

