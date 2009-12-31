#include <Pxf/Pxf.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Util/String.h>
#include <Pxf/Graphics/WindowD3D.h>
#include <Windows.h>

using Pxf::Util::String;

struct my_vertex{
	float x, y, z;
	DWORD colour;
};
const DWORD vertex_fvf=D3DFVF_XYZ|D3DFVF_DIFFUSE;

struct transformed_vertex{
	float x, y, z, rhw;
	DWORD colour;
};
const DWORD transformed_fvf=D3DFVF_XYZRHW|D3DFVF_DIFFUSE;

void draw_fan(IDirect3DDevice9 *_device){
	static transformed_vertex fan[]={ //A coloured fan
		{325,300,1,1,0xFFFFFFFF},
		{250,175,1,1,0xFFFF0000},{300,165,1,1,0xFF7F7F00},{325,155,1,1,0xFF00FF00},
		{375,165,1,1,0xFF007F7F},{400,185,1,1,0xFF0000FF}
	};

	_device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,  //PrimitiveType
		4,                  //PrimitiveCount
		fan,               //pVertexStreamZeroData
		sizeof(transformed_vertex));//VertexStreamZeroStride


}


bool PxfMain(String _CmdLine)
{
	char t_title[512];
	char t_pxftitle[] = "PXF Framework 0.0.1 - Using Direct3D";
	int t_fps = 0;

	Pxf::Graphics::Window *wnd = new Pxf::Graphics::WindowD3D(800, 600, 8, 8, 24, 0);

	wnd->Open();
	wnd->SetTitle(t_pxftitle);
	// Render loop
	while(wnd->IsOpen())
	{

		IDirect3DDevice9* t_device = ((Pxf::Graphics::WindowD3D*)wnd)->GetD3DDevice();
		t_device->SetFVF(transformed_fvf); // Lets get some fixed pipeline up in this bAEtch
		//t_device->Clear()

		// Lets render a simple triangle fan
		draw_fan(t_device);

		// Swap buffers
		wnd->Swap();
		
		// Update title with FPS
		if (t_fps != wnd->GetFPS())
		{
			t_fps = wnd->GetFPS();
			sprintf(t_title, "%s - FPS: %i", t_pxftitle, t_fps);
			wnd->SetTitle(t_title);
		}
		
	}
	wnd->Close();

	delete wnd;

	return true;
}