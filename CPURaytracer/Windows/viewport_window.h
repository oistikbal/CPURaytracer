#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "../imgui_window.h"


namespace cpu_raytracer
{
	class viewport_window : public imgui_window
	{
	public:
		viewport_window();
		~viewport_window();
		void render(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> imageView);
		int m_width;
		int m_height;
	private:
	};
}
