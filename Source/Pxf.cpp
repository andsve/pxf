#include <Pxf/Pxf.h>
#include <Pxf/Util/String.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace Pxf;
using Util::String;

extern bool PxfMain(String _CmdLine);

int main(int argc, const char** argv)
{
	String cmdLine;
	for(size_t i = 0; i < argc; i++)
	{
		cmdLine += argv[i];
		if (i < argc-1)
			cmdLine += " ";
	}

	PxfMain(cmdLine);
	return 0;
}