#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include <d3d11.h>
#include <wrl.h>  // For Microsoft::WRL::ComPtr
#include <memory>

#include "d3dclass.h"

namespace cpu_raytracer
{
    class imgui_impl
    {
    public:
        imgui_impl(HWND hwnd, std::shared_ptr<d3dclass> d3d);
        ~imgui_impl();

        void begin_frame();
        void render();
    private:
        HWND m_hwnd;
        std::shared_ptr<d3dclass> m_d3d;
    };
}