#include <Pxf/Base/Utils.h>

#include <cstdio>

int Pxf::FormatArgumentList(char* _Dest, const char** _Format)
{
	return vsprintf(_Dest, *_Format, (char*)_Format+sizeof(char*));
}

int Pxf::Format(char* _Dest, const char* _Format, ...)
{
	return vsprintf(_Dest, _Format, (char*)(&_Format)+sizeof(char*));
}
