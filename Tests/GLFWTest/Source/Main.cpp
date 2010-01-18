#include <Pxf/Pxf.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Util/String.h>
#include <Pxf/Graphics/OpenGL/WindowGL.h>
#include <Windows.h>

#include <GL/glfw.h>

using Pxf::Util::String;

bool PxfMain(String _CmdLine)
{
	char t_title[512];
	char t_pxftitle[] = "PXF Framework 0.0.1";
	int t_fps = 0;

	Pxf::Graphics::Window *wnd = new Pxf::Graphics::WindowGL(800, 600, 8, 8, 24, 0);

	wnd->Open();
	wnd->SetTitle(t_pxftitle);

	// Render loop
	while(wnd->IsOpen())
	{

		// Swap buffers
		wnd->Swap();
		
		// Update title with FPS
		if (t_fps != wnd->GetFPS())
		{
			t_fps = wnd->GetFPS();
			sprintf(t_title, "%s - %s - FPS: %i", t_pxftitle, wnd->GetContextTypeName(), t_fps);
			wnd->SetTitle(t_title);
		}
		
	}
	wnd->Close();

	delete wnd;

	return true;
}