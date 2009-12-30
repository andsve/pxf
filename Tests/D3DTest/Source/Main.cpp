#include <Pxf/Pxf.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Util/String.h>
#include <Pxf/Graphics/WindowD3D.h>
#include <Windows.h>

using Pxf::Util::String;

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"dxerr9.lib")

bool PxfMain(String _CmdLine)
{
	char t_title[512];
	char t_pxftitle[] = "PXF Framework 0.0.1 - Using Direct3D";
	int t_fps = 0;

	Pxf::Graphics::Window *wnd = new Pxf::Graphics::WindowD3D(800, 600, 8, 8, 24, 0);

	wnd->Open();
	Sleep(2000);
	//wnd->SetTitle(t_pxftitle);

	// Render loop
	/*while(wnd->IsOpen())
	{

		// Swap buffers
		wnd->Swap();
		
		// Update title with FPS
		if (t_fps != wnd->GetFPS())
		{
			t_fps = wnd->GetFPS();
			sprintf(t_title, "%s - FPS: %i", t_pxftitle, t_fps);
			wnd->SetTitle(t_title);
		}
		
	}*/
	wnd->Close();

	delete wnd;

	return true;
}