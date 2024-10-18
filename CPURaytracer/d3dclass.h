#pragma once

#include <d3d11.h>
#include <wrl/client.h>

namespace cpu_raytracer
{
	class d3dclass
	{
	public:
		d3dclass(int screenWidth, int screenHeight, bool vsync, HWND hwnd);
		~d3dclass();

		void render();
		void resize(int width, int height);

		Microsoft::WRL::ComPtr<ID3D11Device> get_device() const;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> get_device_context() const;
		Microsoft::WRL::ComPtr<IDXGISwapChain> get_swap_chain() const;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> get_viewport_view() const;

	private:
		void destroy_render_target();
		void create_render_target(int width, int height);
	private:
		Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
		Microsoft::WRL::ComPtr<ID3D11Device> m_device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deviceContext;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_viewportTexture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_viewportView;
	};
}