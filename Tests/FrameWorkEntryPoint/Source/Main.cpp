#include <Pxf/Pxf.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Util/String.h>

using Pxf::Util::String;

bool PxfMain(String _CmdLine)
{
	PxfStaticAssert(1 == 1, "test");
	return true;
}