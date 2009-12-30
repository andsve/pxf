#include <Pxf/Pxf.h>
#include <Pxf/Util/String.h>
#include <Pxf/Graphics/WindowD3D.h>

using namespace Pxf;
using namespace Pxf::Graphics;
using Util::String;

LRESULT CALLBACK default_window_proc(HWND p_hwnd,UINT p_msg,WPARAM p_wparam,LPARAM p_lparam){

	switch(p_msg){
	  case WM_KEYDOWN:  // A key has been pressed, end the app
	  case WM_CLOSE:    //User hit the Close Window button, end the app
	  case WM_LBUTTONDOWN: //user hit the left mouse button
			printf("HONK HONK\n");
		  return 0;
	}

	return (DefWindowProc(p_hwnd,p_msg,p_wparam,p_lparam));

}

WindowD3D::WindowD3D(int _width, int _height, int _color_bits, int _alpha_bits, int _depth_bits, int _stencil_bits, bool _fullscreen /* = false */, bool _resizeable /* = false */, bool _vsync /* = false */, int _fsaasamples /* = 0 */)
{
	// Window settings
	m_width = _width;
	m_height = _height;
	m_fullscreen = _fullscreen;
	m_resizeable = _resizeable;
	m_vsync = _vsync;
	m_fsaa_samples = _fsaasamples;

	// Buffer bits settings
	m_bits_color = _color_bits;
	m_bits_alpha = _alpha_bits;
	m_bits_depth = _depth_bits;
	m_bits_stencil = _stencil_bits;

	// Win32 poop :O
	m_window = NULL;
	m_D3D=NULL;
	m_D3D_device=NULL;
}

bool WindowD3D::Open()
{
	if (!InitWindow())
		return false;
	
	return InitD3D();
}

bool WindowD3D::Close()
{
	return KillWindow();
}

//////////////////////////////////////////////////////////////////////////
// Private methods below

bool WindowD3D::InitWindow()
{

	HINSTANCE instance=GetModuleHandle(NULL);
	WNDCLASS window_class;
	ULONG window_width, window_height;
	DWORD style;

	window_class.style          = CS_OWNDC;
	window_class.cbClsExtra     = 0;
	window_class.cbWndExtra     = 0;
	window_class.hInstance      = instance;
	window_class.hIcon          = LoadIcon(NULL,IDI_APPLICATION);
	window_class.hCursor        = LoadCursor(NULL,IDC_ARROW);
	window_class.hbrBackground  = (HBRUSH)GetStockObject(BLACK_BRUSH);
	window_class.lpszMenuName   = NULL;
	window_class.lpszClassName  = "PXF Window Class";
	window_class.lpfnWndProc    = default_window_proc;

	//Register the class with windows
	if(!RegisterClass(&window_class)){
		printf("Error registering window class");
		return false;
	}

	

	//If we're running full screen, we cover the desktop with our window.
	//This isn't necessary, but it provides a smoother transition for the
	//user, especially when we're going to change screen modes.
	if(m_fullscreen){
		window_width=GetSystemMetrics(SM_CXSCREEN);
		window_height=GetSystemMetrics(SM_CYSCREEN);
		style=WS_POPUP|WS_VISIBLE;
	}else{
		//In windowed mode, we just make the window whatever size we need.
		window_width=m_width;
		window_height=m_height;
		style=WS_OVERLAPPED|WS_SYSMENU;
	}

	//Here we actually create the window.  For more detail on the various
	//parameters please refer to the Win32 documentation.
	m_window=CreateWindow("PXF Window Class", //name of our registered class
		"PXF Framework", //Window name/title
		style,      //Style flags
		0,          //X position
		0,          //Y position
		window_width,//width of window
		window_height,//height of window
		NULL,       //Parent window
		NULL,       //Menu
		instance, //application instance handle
		NULL);      //pointer to window-creation data


	//Hide the mouse in full-screen
	if(m_fullscreen){
		ShowCursor(FALSE);
	}

	//When you request a window of a certain width, that width includes the
	// borders, so we adjust the window so the client (drawable) area is
	// as big as we requested
	// Also, by querying for the display size we can center the window
	if(!m_fullscreen){
		RECT client_rect;
		RECT window_rect;
		ULONG window_width,window_height;
		ULONG screen_width,screen_height;
		ULONG new_x, new_y;

		GetClientRect(m_window,&client_rect);
		GetWindowRect(m_window,&window_rect);

		window_width = window_rect.right - window_rect.left;
		window_height = window_rect.bottom - window_rect.top;

		window_width = window_width + (window_width - client_rect.right);
		window_height = window_height + (window_height - client_rect.bottom);

		screen_width=GetSystemMetrics(SM_CXSCREEN);
		screen_height=GetSystemMetrics(SM_CYSCREEN);

		new_x = (screen_width - window_width) / 2;
		new_y = (screen_height - window_height) / 2;

		MoveWindow(m_window,
			new_x, //Left edge
			new_y, //Top Edge
			window_width,    //New Width
			window_height, //New Height
			TRUE);
	}

	//Now that all adjustments have been made, we can show the window
	ShowWindow(m_window,SW_SHOW);

	return true;
}

bool WindowD3D::KillWindow()
{
	HINSTANCE instance=GetModuleHandle(NULL);

	//Test if our window is valid
	if(m_window){
		if(!DestroyWindow(m_window)){
			//We failed to destroy our window, this shouldn't ever happen
			printf("Destroy Window Failed");
		}else{
			MSG msg;
			//Clean up any pending messages
			while(PeekMessage(&msg, NULL, 0, 0,PM_REMOVE)){
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	m_window=NULL;

	//Unregister our window, if we had opened multiple windows using this
	//class, we would have to close all of them before we unregistered the class.
	if(!UnregisterClass("PXF Window Class",instance)){
		printf("Error unregistering window class");
	}

	return true;
}

bool WindowD3D::InitD3D()
{
	//Create Direct3D9, this is the first thing you have to do in any D3D9 program
	//Always pass D3D_SDK_VERSION to the function.
	m_D3D = Direct3DCreate9( D3D_SDK_VERSION);
	if(!m_D3D)
	{
		return false;
	}


	//Find a good display/pixel format
	D3DFORMAT t_format;
	/* TODO: Fix this SH*T...
	if(m_fullscreen)
	{
		switch(m_bits_depth)
		{
		case 16:
			t_format=dhFind16BitMode(m_D3D);
			break;
		case 32:
			t_format=dhFind32BitMode(m_D3D);
			break;
		default:
			dhLog("dhGetFormat: called with invalid depth");
			t_format = D3DFMT_UNKNOWN;
			break;
		}
	}
	else
	{*/
		t_format = D3DFMT_UNKNOWN; //D3D will automatically use the Desktop's format
	//}

	DWORD adapter;
	m_D3D->GetAdapterIdentifier(0, 0, (D3DADAPTER_IDENTIFIER9*)&adapter);
	D3DDEVTYPE dev_type = D3DDEVTYPE_HAL;//user_prefs.GetDeviceType();

	//Clear out our D3DPRESENT_PARAMETERS structure.  Even though we're going
	//to set virtually all of its members, it's good practice to zero it out first.
	ZeroMemory(&m_pp,sizeof(D3DPRESENT_PARAMETERS));

	//Whether we're full-screen or windowed these are the same.
	m_pp.BackBufferCount	= 1;  //We only need a single back buffer
	m_pp.MultiSampleType	= D3DMULTISAMPLE_NONE; //No multi-sampling
	m_pp.MultiSampleQuality	= 0; //No multi-sampling
	m_pp.SwapEffect			= D3DSWAPEFFECT_DISCARD; // Throw away previous frames, we don't need them
	m_pp.hDeviceWindow		= m_window;  //This is our main (and only) window
	m_pp.Flags				= 0;  //No flags to set
	m_pp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT; //Default Refresh Rate
	m_pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE; //Default Presentation rate
	m_pp.BackBufferFormat  = t_format; //Display format

	m_pp.EnableAutoDepthStencil = true;
	m_pp.AutoDepthStencilFormat = D3DFMT_UNKNOWN; //This is ignored if EnableAutoDepthStencil is FALSE

	//BackBufferWidth/Height have to be set for full-screen applications, these values are
	//used (along with BackBufferFormat) to determine the display mode.
	//They aren't needed in windowed mode since the size of the window will be used.
	if(m_fullscreen)
	{
		m_pp.Windowed			= FALSE;
		m_pp.BackBufferWidth	= m_width;
		m_pp.BackBufferHeight  = m_height;
	}
	else
	{
		m_pp.Windowed = TRUE;
	}

	//Create our device
	m_D3D->CreateDevice((UINT)adapter, //User specified adapter, on a multi-monitor system
		//there can be more than one.
		//User specified device type, Usually HAL
		dev_type,
		//Our Window
		m_window,
		//Process vertices in software. This is slower than in hardware,
		//But will work on all graphics cards.
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		//Our D3DPRESENT_PARAMETERS structure, so it knows what we want to build
		&m_pp,
		//This will be set to point to the new device
		&m_D3D_device);

	return true;


}