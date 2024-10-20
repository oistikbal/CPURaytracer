#include "..\imgui\imgui.h"
#include "settings_window.h"

cpu_raytracer::settings_window::settings_window(std::shared_ptr<viewport_window> viewportWindow) : imgui_window("Settings"), m_viewportWindow(viewportWindow), m_resolutionScaler(10)
{
}

cpu_raytracer::settings_window::~settings_window()
{
}

void cpu_raytracer::settings_window::render(float deltaTime)
{
	if (m_open)
	{
		ImGui::Begin(m_title.c_str(), &m_open);

		if (ImGui::CollapsingHeader("Stats"))
		{
			ImGui::Text("Fps:");
			ImGui::SameLine();
			ImGui::Text("%.2f", 1.0f / deltaTime);
		}

		if (ImGui::CollapsingHeader("Resolution")) 
		{
			ImGui::Text("Viewport Resolution: X: %i, Y: % i", m_viewportWindow->get_width(), m_viewportWindow->get_height());
			ImGui::Text("Aspect Ratio: %.3f", (float)m_viewportWindow->get_height() / m_viewportWindow->get_width());

			ImGui::Text("Adjust Resolution Scale:");
			ImGui::SliderInt("##Slider", &m_resolutionScaler, 0, 100);

		}

        ImGui::End();
	}
}
