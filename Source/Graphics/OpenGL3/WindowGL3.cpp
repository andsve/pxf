#include <Pxf/Pxf.h>
#include <Pxf/Base/Platform.h>
#include <Pxf/Util/String.h>
#include <Pxf/Graphics/OpenGL3/WindowGL3.h>
#include <Pxf/Base/Debug.h>

#ifdef CONF_PLATFORM_MACOSX
#include <Carbon/Carbon.h>
#endif
#include <GL/glfw.h>

using namespace Pxf;
using namespace Pxf::Graphics;
using Util::String;

int WindowGL3::GetWidth() { return m_width; }
int WindowGL3::GetHeight() {return m_height; }
float WindowGL3::GetAspectRatio() { return ((float)m_width / (float)m_height); }

WindowGL3::WindowGL3(WindowSpecifications *_window_spec)
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
	m_fps_laststamp = Platform::GetTime();

	#ifdef CONF_FAMILY_WINDOWS
	// reset windows params
	m_HDC = NULL;
	m_HRC = NULL;
	m_Window = NULL;
	m_HInstance = NULL;
	#endif //CONF_FAMILY_WINDOWS
}

WindowGL3::~WindowGL3()
{
	Close();
}

bool WindowGL3::InitWindow()
{
	#ifdef CONF_FAMILY_WINDOWS
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
	//window_class.lpfnWndProc    = default_window_proc; // aoe :(

	//Register the class with windows
	if(!RegisterClass(&window_class)){
		Message("WindowGL3", "Error registering window class!");
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
	m_Window=CreateWindow("PXF Window Class", //name of our registered class
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

		GetClientRect(m_Window,&client_rect);
		GetWindowRect(m_Window,&window_rect);

		window_width = window_rect.right - window_rect.left;
		window_height = window_rect.bottom - window_rect.top;

		window_width = window_width + (window_width - client_rect.right);
		window_height = window_height + (window_height - client_rect.bottom);

		screen_width=GetSystemMetrics(SM_CXSCREEN);
		screen_height=GetSystemMetrics(SM_CYSCREEN);

		new_x = (screen_width - window_width) / 2;
		new_y = (screen_height - window_height) / 2;

		MoveWindow(m_Window,
			new_x, //Left edge
			new_y, //Top Edge
			window_width,    //New Width
			window_height, //New Height
			TRUE);
	}

	//Now that all adjustments have been made, we can show the window
	ShowWindow(m_Window,SW_SHOW);
	
	//#else
	// if not windows architechture, use glfw (temporary solution..)
	#endif

	return true;
}


bool WindowGL3::Open()
{
	if (!InitWindow())
		return false;
	
	//if (!InitD3D())
	//	return false;

	//Message("WindowD3D9", "Opened window of %dx%d@%d (r: %d g: %d b: %d a: %d d: %d s: %d)", m_width, m_height, m_bits_color*3+m_bits_alpha, m_bits_color, m_bits_color, m_bits_color, m_bits_alpha, m_bits_depth, m_bits_stencil);

	//m_D3D_device->BeginScene();
	return true;

	/*
	int t_params = GLFW_WINDOW;

	if (IsOpen())
		return false; // can't open an already open window

	// Enable fullscreen
	if (m_fullscreen)
		t_params = GLFW_FULLSCREEN;

	// Enable vertical sync
	if (m_vsync)
		glfwSwapInterval(1);

	// Set number of FSAA samples
	if (m_fsaa_samples > 0)
		glfwOpenWindowHint(GLFW_FSAA_SAMPLES, m_fsaa_samples);

	if (GL_TRUE == glfwOpenWindow(m_width, m_height, m_bits_color, m_bits_color, m_bits_color, m_bits_alpha, m_bits_depth, m_bits_stencil, t_params))
	{

#ifdef CONF_PLATFORM_MACOSX
		/* HACK - Get events without bundle */
	/*
		ProcessSerialNumber psn;    
		GetCurrentProcess(&psn);
		TransformProcessType(&psn,kProcessTransformToForegroundApplication);
		SetFrontProcess(&psn);
#endif
		const GLubyte* ver = glGetString(GL_VERSION);
		const GLubyte* vend = glGetString(GL_VENDOR);
		const GLubyte* ext = glGetString(GL_EXTENSIONS);


		Message("WindowGL3", "Opened window of %dx%d@%d (r: %d g: %d b: %d a: %d d: %d s: %d)", m_width, m_height, m_bits_color*3+m_bits_alpha, m_bits_color, m_bits_color, m_bits_color, m_bits_alpha, m_bits_depth, m_bits_stencil);

		return true;
	}
	else
		return false;

		*/

	return true;
}

bool WindowGL3::Close()
{
	if(!IsOpen())
		return false;

	glfwCloseWindow();
	return true;
}

void WindowGL3::Swap()
{
	if (IsOpen())
	{
		int64 diff;
		int64 t_current_time = Platform::GetTime();
		diff = t_current_time - m_fps_laststamp;
		if (diff >= 1000)
		{
			m_fps = m_fps_count;
			m_fps_count = 0;
			m_fps_laststamp = t_current_time;
		}

		glfwSwapBuffers();
		m_fps_count += 1;
	}
}

bool WindowGL3::IsOpen()
{
	if (glfwGetWindowParam(GLFW_OPENED) == GL_TRUE)
		return true;
	else
		return false;
}

bool WindowGL3::IsActive()
{
	if (IsOpen())
	{
		if (GL_TRUE == glfwGetWindowParam(GLFW_ACTIVE))
			return true;
	}
	return false;
}

bool WindowGL3::IsMinimized()
{
	if (IsOpen())
	{
		if (GL_TRUE == glfwGetWindowParam(GLFW_ICONIFIED))
			return true;
	}

	return false;
}

void WindowGL3::SetTitle(const char *_title)
{
	if (IsOpen())
	{
		glfwSetWindowTitle(_title);
	}
}

int WindowGL3::GetFPS()
{
	return m_fps;
}

char* WindowGL3::GetContextTypeName()
{
	return "OpenGL";
}