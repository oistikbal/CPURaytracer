#include "system.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include <chrono>

static cpu_raytracer::system* g_system;

cpu_raytracer::system::system()
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;


	// Get an external pointer to this object.	
	g_system = this;

	// Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	m_applicationName = L"CPURaytracer";

	// Setup the windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	int screenWidth = 1366;
	int screenHeight = 768;

	// Place the window in the middle of the screen.
	posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
	posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_OVERLAPPEDWINDOW,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	m_d3d = std::make_shared<d3dclass>(screenWidth, screenHeight, true, m_hwnd);
	m_imgui = std::make_shared<imgui_impl>(m_hwnd, m_d3d);
	m_application = std::make_shared<cpu_raytracer::application>();
}

cpu_raytracer::system::~system()
{
	shutdown_windows();
}

void cpu_raytracer::system::run()
{
	MSG msg;
	bool done, result;


	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message from the window or the user.
	done = false;
	while (!done)
	{
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			static auto previous_time = std::chrono::high_resolution_clock::now();
			auto current_time = std::chrono::high_resolution_clock::now();
			std::chrono::duration<float> delta_time = current_time - previous_time;
			previous_time = current_time;

			result = frame(delta_time.count());
			if (!result)
			{
				done = true;
			}
		}

	}

	return;
}

bool cpu_raytracer::system::frame(float deltaTime)
{
	if (!m_application->frame(deltaTime))
	{
		return false;
	}

	m_d3d->render();
	m_imgui->begin_frame();
	m_imgui->render(deltaTime);
	m_d3d->get_swap_chain()->Present(1, 0);

	return true;
}


void cpu_raytracer::system::shutdown_windows()
{
	// Show the mouse cursor.
	ShowCursor(true);
	m_hwnd = nullptr;

	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = nullptr;
	g_system = nullptr;

	return;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK cpu_raytracer::system::message_handler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_MOUSEWHEEL:
		//m_input->update(hwnd, umsg, wparam, lparam);
		break;

	case WM_SIZE:
	{
		if (m_application == nullptr)
			return 0;

		UINT width = LOWORD(lparam);
		UINT height = HIWORD(lparam);
		m_application->resize(width, height);
		m_d3d->resize(width, height);
	}

	default:
	{
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, umessage, wparam, lparam))
		return true;


	switch (umessage)
	{
		// Check if the window is being destroyed.
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	// Check if the window is being closed.
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}


	// All other messages pass to the message handler in the system class.
	default:
	{
		return g_system->message_handler(hwnd, umessage, wparam, lparam);
	}
	}
}