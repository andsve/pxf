#include <Pxf/Pxf.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Clock.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Util/String.h>

#include <windows.h>

using Pxf::Util::String;

bool PxfMain(String _CmdLine)
{
	PXFSTATICASSERT(1 == 1, "test");
	PXFASSERT(1 == 1, "test");

	Pxf::Message("Main", "Test %.2f", 3.33f);

	Pxf::Clock c1;
	int64 t1 = c1.GetTime();
	Sleep(5005);
	int64 t2 = c1.GetTime();
	int64 diff = t2 - t1;

	

	return true;
}