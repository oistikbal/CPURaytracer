#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <memory>

#include "Core/application.h"
#include "d3dclass.h"
#include "imgui_impl.h"

namespace cpu_raytracer 
{
	class system
	{
	public:
		system();
		~system();

		void run();

		LRESULT CALLBACK message_handler(HWND, UINT, WPARAM, LPARAM);
	private:
		bool frame(float deltaTime);
		void shutdown_windows();

	private:
		LPCWSTR m_applicationName;
		HINSTANCE m_hinstance;
		HWND m_hwnd;

		std::shared_ptr<application> m_application;
		std::shared_ptr<imgui_impl> m_imgui;
		std::shared_ptr<d3dclass> m_d3d;
	};
}

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);