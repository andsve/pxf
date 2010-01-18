#include <Pxf/Pxf.h>
#ifdef CONF_FAMILY_WINDOWS
#include <Pxf/Base/Clock.h>
#include <Pxf/Util/String.h>
#include <Pxf/Graphics/D3D9/WindowD3D.h>
#include <Pxf/Base/Debug.h>

using namespace Pxf;
using namespace Pxf::Graphics;
using Util::String;

int WindowD3D::GetWidth() { return m_width; }
int WindowD3D::GetHeight() {return m_height; }
float WindowD3D::GetAspectRatio() { return ((float)m_width / (float)m_height); }

LRESULT CALLBACK default_window_proc(HWND p_hwnd,UINT p_msg,WPARAM p_wparam,LPARAM p_lparam){

	switch(p_msg){
	  case WM_KEYDOWN:  // A key has been pressed, end the app
	  case WM_CLOSE:    //User hit the Close Window button, end the app
	  case WM_DESTROY:
		  PostQuitMessage(WM_QUIT);
		  break;
	  case WM_LBUTTONDOWN: //user hit the left mouse button
		  return 0;
	}

	return (DefWindowProc(p_hwnd,p_msg,p_wparam,p_lparam));

}

WindowD3D::WindowD3D(WindowSpecifications *_window_spec)
{
	// Window settings
	m_width = _window_spec->Width;
	m_height = _window_spec->Height;
	m_fullscreen = _window_spec->Fullscreen;
	m_resizeable = _window_spec->Resizeable;
	m_vsync = _window_spec->VerticalSync;
	m_fsaa_samples = _window_spec->FSAASamples;

	// Buffer bits settings
	m_bits_color = _window_spec->ColorBits;
	m_bits_alpha = _window_spec->AlphaBits;
	m_bits_depth = _window_spec->DepthBits;
	m_bits_stencil = _window_spec->StencilBits;

	// FPS
	m_fps = 0;
	m_fps_count = 0;
	m_fps_laststamp = Clock::GetTime();

	// Win32 poop :O
	m_window = NULL;
	m_D3D=NULL;
	m_D3D_device=NULL;
}

WindowD3D::~WindowD3D()
{
	Close();
}

bool WindowD3D::Open()
{
	if (!InitWindow())
		return false;
	
	if (!InitD3D())
		return false;

	Message("WindowD3D9", "Opened window of %dx%d@%d (r: %d g: %d b: %d a: %d d: %d s: %d)", m_width, m_height, m_bits_color*3+m_bits_alpha, m_bits_color, m_bits_color, m_bits_color, m_bits_alpha, m_bits_depth, m_bits_stencil);

	m_D3D_device->BeginScene();
	return true;
}

bool WindowD3D::Close()
{
	if (!KillD3D())
		return false;

	if (!KillWindow())
		return false;

	return true;
}

void WindowD3D::Swap()
{
	HRESULT hr=m_D3D_device->TestCooperativeLevel();
	//Our device is lost
	//We've lost our device and it cannot be Reset yet.
	if(hr == D3DERR_DEVICELOST){
		Close();
		return;
	}else if(hr == D3DERR_DEVICENOTRESET){ //The device is ready to be Reset
		hr=m_D3D_device->Reset(&m_pp);
	}
	
	if(FAILED(hr)){
		Close();
	}

	MSG msg;
	while(PeekMessage(&msg, NULL, 0, 0,PM_REMOVE)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// Swap stuff (?)
	m_D3D_device->EndScene();
	m_D3D_device->Present(NULL, NULL, NULL, NULL);

	// FPS
	int64 diff;
	int64 t_current_time = Clock::GetTime();
	diff = t_current_time - m_fps_laststamp;
	if (diff >= 1000)
	{
		m_fps = m_fps_count;
		m_fps_count = 0;
		m_fps_laststamp = t_current_time;
	}

	m_D3D_device->BeginScene();
	m_fps_count += 1;

}

int WindowD3D::GetFPS()
{
	return m_fps;
}

char* WindowD3D::GetContextTypeName()
{
	return "Direct3D9";
}

void WindowD3D::SetTitle(const char *_title)
{
	if (IsOpen())
	{
		LPCTSTR t_title;
		t_title = _title;
		SetWindowText(m_window, t_title);
	}
}

bool WindowD3D::IsOpen()
{
	if (GetWindow(m_window, GW_HWNDFIRST) == NULL)
		return false;

	return true;
	//return m_open;
}

//////////////////////////////////////////////////////////////////////////
// Specific D3D getters

IDirect3DDevice9* WindowD3D::GetD3DDevice()
{
	return m_D3D_device;
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
		Message("WindowD3D9", "Error registering window class!");
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
		if (m_resizeable)
			style=WS_OVERLAPPEDWINDOW|WS_SYSMENU;//WS_OVERLAPPED|WS_SYSMENU;
		else
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
	if (IsOpen()) {
		if(!DestroyWindow(m_window)){
			//We failed to destroy our window, this shouldn't ever happen
			Message("WindowD3D9", "DestroyWindow() failed!");
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
		Message("WindowD3D9", "Error unregistering window class!");
	}

	return true;
}

bool WindowD3D::InitD3D()
{
	HRESULT hr;
	m_D3D = Direct3DCreate9( D3D_SDK_VERSION);
	if(m_D3D == NULL)
	{
		Message("WindowD3D9", "Direct3DCreate9() failed!");
		return false;
	}

	//Clear out our D3DPRESENT_PARAMETERS structure.  Even though we're going
	//to set virtually all of its members, it's good practice to zero it out first.
	ZeroMemory(&m_pp,sizeof(D3DPRESENT_PARAMETERS));

	//Whether we're full-screen or windowed these are the same.
	m_pp.BackBufferCount = 1;  //We only need a single back buffer
	if (m_fsaa_samples == 2)
		m_pp.MultiSampleType	= D3DMULTISAMPLE_2_SAMPLES;
	else if (m_fsaa_samples == 3)
		m_pp.MultiSampleType	= D3DMULTISAMPLE_3_SAMPLES;
	else if (m_fsaa_samples == 4)
		m_pp.MultiSampleType	= D3DMULTISAMPLE_4_SAMPLES;
	else if (m_fsaa_samples == 5)
		m_pp.MultiSampleType	= D3DMULTISAMPLE_5_SAMPLES;
	else if (m_fsaa_samples == 6)
		m_pp.MultiSampleType	= D3DMULTISAMPLE_6_SAMPLES;
	else if (m_fsaa_samples == 7)
		m_pp.MultiSampleType	= D3DMULTISAMPLE_7_SAMPLES;
	else if (m_fsaa_samples == 8)
		m_pp.MultiSampleType	= D3DMULTISAMPLE_8_SAMPLES;
	else if (m_fsaa_samples == 9)
		m_pp.MultiSampleType	= D3DMULTISAMPLE_9_SAMPLES;
	else if (m_fsaa_samples == 10)
		m_pp.MultiSampleType	= D3DMULTISAMPLE_10_SAMPLES;
	else if (m_fsaa_samples == 11)
		m_pp.MultiSampleType	= D3DMULTISAMPLE_11_SAMPLES;
	else if (m_fsaa_samples == 12)
		m_pp.MultiSampleType	= D3DMULTISAMPLE_12_SAMPLES;
	else if (m_fsaa_samples == 13)
		m_pp.MultiSampleType	= D3DMULTISAMPLE_13_SAMPLES;
	else if (m_fsaa_samples == 14)
		m_pp.MultiSampleType	= D3DMULTISAMPLE_14_SAMPLES;
	else if (m_fsaa_samples == 15)
		m_pp.MultiSampleType	= D3DMULTISAMPLE_15_SAMPLES;
	else if (m_fsaa_samples == 16)
		m_pp.MultiSampleType	= D3DMULTISAMPLE_16_SAMPLES;
	else {
		m_pp.MultiSampleType	= D3DMULTISAMPLE_NONE; //No multi-sampling
	}
	m_pp.MultiSampleQuality	= 0; //No multi-sampling (?)
	m_pp.SwapEffect			= D3DSWAPEFFECT_FLIP; // Throw away previous frames, we don't need them
	m_pp.hDeviceWindow		= m_window;  //This is our main (and only) window
	m_pp.Flags				= 0;  //No flags to set
	m_pp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT; //Default Refresh Rate
	if (m_vsync)
		m_pp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	else
		m_pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	m_pp.BackBufferFormat  = D3DFMT_UNKNOWN; //Display format

	m_pp.EnableAutoDepthStencil = false;
	m_pp.AutoDepthStencilFormat = D3DFMT_UNKNOWN; //This is ignored if EnableAutoDepthStencil is FALSE

	//BackBufferWidth/Height have to be set for full-screen applications, these values are
	//used (along with BackBufferFormat) to determine the display mode.
	//They aren't needed in windowed mode since the size of the window will be used.
	if(m_fullscreen)
	{
		m_pp.Windowed			= FALSE;
		m_pp.BackBufferWidth	= m_width;
		m_pp.BackBufferHeight	= m_height;
	}
	else
	{
		m_pp.Windowed = TRUE;
	}

	//Create our device
	hr = m_D3D->CreateDevice(D3DADAPTER_DEFAULT, //User specified adapter, on a multi-monitor system
		//there can be more than one.
		//User specified device type, Usually HAL
		D3DDEVTYPE_HAL,
		//Our Window
		m_window,
		//Process vertices in software. This is slower than in hardware,
		//But will work on all graphics cards.
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		//Our D3DPRESENT_PARAMETERS structure, so it knows what we want to build
		&m_pp,
		//This will be set to point to the new device
		&m_D3D_device);

	if (FAILED(hr))
	{
		if (hr == D3DERR_DEVICELOST)
			Message("WindowD3D9", "CreateDevice() failed: Device lost!");
		else if (hr == D3DERR_INVALIDCALL)
			Message("WindowD3D9", "CreateDevice() failed: Invalid call!");
		else if (hr == D3DERR_NOTAVAILABLE)
			Message("WindowD3D9", "CreateDevice() failed: Not available!");
		else if (hr == D3DERR_OUTOFVIDEOMEMORY)
			Message("WindowD3D9", "CreateDevice() failed: Out of video memory!");
		else
			Message("WindowD3D9", "CreateDevice() failed: Unknown error!");
		
		return false;
	}


	return true;


}

bool WindowD3D::KillD3D()
{
	if(m_D3D_device)
	{
		m_D3D_device->Release();
		m_D3D_device = NULL;
	}

	if(m_D3D)
	{
		m_D3D->Release();
		m_D3D = NULL;
	}

	return true;
}

#endif // CONF_FAMILY_WINDOWS