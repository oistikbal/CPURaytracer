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
		m_width = ImGui::GetContentRegionAvail().x;
		m_height = ImGui::GetContentRegionAvail().y;
		ImGui::Image(
			reinterpret_cast<ImTextureID>(imageView.Get()),
			ImGui::GetContentRegionAvail(),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);
		ImGui::End();
    }
}
