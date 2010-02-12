#include <pxf/Base/Config.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>

#include <cstdio>

#ifdef CONF_PLATFORM_MACOSX
#include <cstdarg>
#endif

using namespace Pxf;

bool Pxf::Assert(bool _Expression, int _Line, char* _File, const char* _Message, ...)
{
	if (!_Expression)
	{
		char Buffer[4092];
		FormatArgumentList(Buffer, &_Message);
		printf("Assertion at %s:%d: %s\n", _File, _Line, Buffer);
		return false;
	}
	return true;
}

void Pxf::Message(const char* _SubSystem, const char* _Message, ...)
{
	char Buffer[4092];
#ifdef CONF_PLATFORM_MACOSX
	va_list va;
	va_start(va, _Message);
	vsprintf(Buffer, _Message, va);
	printf("[%s] %s\n", _SubSystem, Buffer);
	va_end(va);
#else
	FormatArgumentList(Buffer, &_Message);
	printf("[%s] %s\n", _SubSystem, Buffer);
	fflush(stdout);
#endif
}