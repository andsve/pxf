#include <Pxf/Pxf.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Util/String.h>
#include <Pxf/Graphics/WindowGL.h>
#include <Windows.h>

#include <GL/glfw.h>

using Pxf::Util::String;

bool PxfMain(String _CmdLine)
{
	char t_title[512];
	char t_pxftitle[] = "PXF Framework 0.0.1";;

	Pxf::Graphics::Window *wnd = new Pxf::Graphics::WindowGL(800, 600, 8, 8, 24, 0);

	wnd->Open();
	wnd->SetTitle(t_pxftitle);

	// Render loop
	while(wnd->IsOpen())
	{

		// Swap buffers
		wnd->Swap();
		
		// Update title with FPS
		sprintf(t_title, "%s - FPS: %i", t_pxftitle, wnd->GetFPS());
		wnd->SetTitle(t_title);
	}
	wnd->Close();

	delete wnd;

	return true;
}