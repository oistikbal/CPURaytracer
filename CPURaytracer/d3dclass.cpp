#include "d3dclass.h"
#include <stdexcept>

using namespace Microsoft::WRL;

cpu_raytracer::d3dclass::d3dclass(int screenWidth, int screenHeight, bool vsync, HWND hwnd)
{
    ComPtr<IDXGIFactory> factory;
    ComPtr<IDXGIAdapter> adapter;
    ComPtr<IDXGIOutput> adapterOutput;

    DXGI_MODE_DESC* displayModeList = nullptr;
    D3D_FEATURE_LEVEL featureLevel;
    unsigned int numModes = 0, i = 0, numerator = 0, denominator = 0;
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};


    auto result = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(factory.GetAddressOf()));
    if (FAILED(result))
        throw std::runtime_error("Failed to create DXGI Factory");

    // Enumerate adapters
    result = factory->EnumAdapters(0, adapter.GetAddressOf());
    if (FAILED(result))
        throw std::runtime_error("Failed to enumerate adapters");

    // Enumerate outputs
    result = adapter->EnumOutputs(0, adapterOutput.GetAddressOf());
    if (FAILED(result))
        throw std::runtime_error("Failed to enumerate outputs");

    // Get display mode list
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
    if (FAILED(result))
        throw std::runtime_error("Failed to get display mode list");

    displayModeList = new DXGI_MODE_DESC[numModes];
    if (!displayModeList)
        throw std::runtime_error("Failed to allocate memory for display mode list");

    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
    if (FAILED(result))
        throw std::runtime_error("Failed to get display mode list with details");

    // Find matching refresh rate and resolution
    for (i = 0; i < numModes; i++)
    {
        if (displayModeList[i].Width == (unsigned int)screenWidth && displayModeList[i].Height == (unsigned int)screenHeight)
        {
            numerator = displayModeList[i].RefreshRate.Numerator;
            denominator = displayModeList[i].RefreshRate.Denominator;
        }
    }

    factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);


    delete[] displayModeList;
    displayModeList = nullptr;


    RECT clientRect;
    GetClientRect(hwnd, &clientRect);

    // The width and height of the renderable area
    int renderWidth = clientRect.right - clientRect.left;
    int renderHeight = clientRect.bottom - clientRect.top;

    // Initialize swap chain description
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = renderWidth;
    swapChainDesc.BufferDesc.Height = renderHeight;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    if (vsync)
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
    }
    else
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    }

    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hwnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Windowed = true;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = 0;

    featureLevel = D3D_FEATURE_LEVEL_11_1;

	result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, m_swapChain.GetAddressOf(), m_device.GetAddressOf(), nullptr, m_deviceContext.GetAddressOf());
	if (FAILED(result))
		throw std::runtime_error("Failed to create device and swap chain");
    create_render_target();
}

cpu_raytracer::d3dclass::~d3dclass()
{
}

void cpu_raytracer::d3dclass::render()
{
    const float clear_color_with_alpha[4] = { 0.0f, 0.0f, 0.0f, 1.0f};
    m_deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), nullptr);
    m_deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), clear_color_with_alpha);
}

void cpu_raytracer::d3dclass::resize(int width, int height)
{
    destroy_render_target();
    m_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    create_render_target();
}

Microsoft::WRL::ComPtr<ID3D11Device> cpu_raytracer::d3dclass::get_device() const
{
    return m_device;
}

Microsoft::WRL::ComPtr<ID3D11DeviceContext> cpu_raytracer::d3dclass::get_device_context() const
{
    return m_deviceContext;
}

Microsoft::WRL::ComPtr<IDXGISwapChain> cpu_raytracer::d3dclass::get_swap_chain() const
{
    return m_swapChain;
}

void cpu_raytracer::d3dclass::destroy_render_target()
{
    if (m_renderTargetView)
        m_renderTargetView->Release();
}

void cpu_raytracer::d3dclass::create_render_target()
{
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
    m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf());
}
