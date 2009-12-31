#include <Pxf/Pxf.h>
#include <Pxf/Base/Clock.h>
#include <Pxf/Util/String.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <GL/glfw.h>

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

	// Initialize GLFW
	// TODO: Sanity check glfw-errors etc via our own error handling.
	if (glfwInit() != GL_TRUE)
	{
		printf("Could not initialize GLFW!\n");
		return EXIT_FAILURE;
	}

	Clock::Clock();

	PxfMain(cmdLine);

	glfwTerminate();

	return 0;
}