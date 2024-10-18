#pragma once
#include <string>

namespace cpu_raytracer
{
    class imgui_window
    {
    public:
        imgui_window(const std::string& title) : m_title(title), m_open(true) {}
        bool is_open() const { return m_open; }
        void set_open(bool isOpen) { m_open = isOpen; }
    protected:
        std::string m_title;
        bool m_open; // Window open status
    };
}