#include "../imgui/imgui.h"
#include "viewport_window.h"

cpu_raytracer::viewport_window::viewport_window() : imgui_window::imgui_window("Viewport"), m_height(0), m_width(0)
{
}

cpu_raytracer::viewport_window::~viewport_window()
{

}

void cpu_raytracer::viewport_window::render(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> imageView)
{
    if (m_open)
    {
        ImGui::Begin(m_title.c_str(), &m_open);
		auto viewportSize = ImGui::GetContentRegionAvail();
		ImGui::Image(
			(ImTextureID)(void*)imageView.Get(),
			viewportSize,
			ImVec2(0.0f, 0.0f),
			ImVec2(1.0f, 1.0f)
		);
		ImGui::End();
		m_height = viewportSize.y;
		m_width = viewportSize.x;
    }
}

int cpu_raytracer::viewport_window::get_width() const
{
	return m_width;
}

int cpu_raytracer::viewport_window::get_height() const
{
	return m_height;
}
