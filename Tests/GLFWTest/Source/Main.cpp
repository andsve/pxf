#include <Pxf/Pxf.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Util/String.h>
#include <Pxf/Graphics/WindowGL.h>

using Pxf::Util::String;

bool PxfMain(String _CmdLine)
{
	Pxf::Graphics::Window *wnd = new Pxf::Graphics::WindowGL(800, 600);

	wnd->Open();
	printf("Do nothing?\n");
	wnd->Close();

	delete wnd;

	return true;
}