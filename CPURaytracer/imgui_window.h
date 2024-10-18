#pragma once
#include <string>

namespace cpu_raytracer
{
    class imgui_window
    {
    public:
        imgui_window(const std::string& title) : m_title(title), m_open(true) {}

        virtual void render() = 0;

        bool is_open() const { return m_open; }

    protected:
        std::string m_title;
        bool m_open; // Window open status
    };
}