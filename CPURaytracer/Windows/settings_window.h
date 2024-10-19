#pragma once
#include "..\imgui_window.h"
#include "viewport_window.h"
#include <memory>

namespace cpu_raytracer 
{
	class settings_window : public imgui_window
	{
	public:
		settings_window(std::shared_ptr<viewport_window> viewportWindow);
		~settings_window();
		void render(float deltaTime);

	private:
		std::shared_ptr<viewport_window> m_viewportWindow;
		int m_resolutionScaler;
	};

}
