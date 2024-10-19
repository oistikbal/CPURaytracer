#include "d3dclass.h"
#include <stdexcept>
#include <vector>

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

    create_render_target(renderWidth, renderHeight);
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
    create_render_target(width, height);
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

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cpu_raytracer::d3dclass::get_viewport_view() const
{
    return m_viewportView;
}

void cpu_raytracer::d3dclass::destroy_render_target()
{
    if (m_renderTargetView)
        m_renderTargetView.Reset();

    if (m_viewportTexture) 
        m_viewportTexture.Reset();
    if (m_viewportView) 
        m_viewportView.Reset();
}

void cpu_raytracer::d3dclass::create_render_target(int width, int height)
{
    if (width < 0 || height < 0)
        return;


    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
    m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf());

    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = width;         // Width of the texture
    textureDesc.Height = height;       // Height of the texture
    textureDesc.MipLevels = 1;         // Number of mipmap levels
    textureDesc.ArraySize = 1;         // Number of textures in the array
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Texture format
    textureDesc.SampleDesc.Count = 1;  // No multi-sampling
    textureDesc.Usage = D3D11_USAGE_DEFAULT; // Usage flag
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // Bind the texture for shader use

    HRESULT hr = m_device->CreateTexture2D(&textureDesc, nullptr, m_viewportTexture.GetAddressOf());

    m_device->CreateShaderResourceView(m_viewportTexture.Get(), nullptr, m_viewportView.GetAddressOf());

    UINT solidColor = 0xffffffff; // ARGB format: Red

    // Create an array to hold the color data
    std::vector<UINT> colorData(width * height);

    for (UINT y = 0; y < height; ++y)
    {
        for (UINT x = 0; x < width; ++x)
        {
            // Normalize coordinates (0 to 1 range)
            float normX = static_cast<float>(x) / (width - 1);
            float normY = static_cast<float>(y) / (height - 1);

            // Calculate the color components based on normalized coordinates
            UINT r = static_cast<UINT>(normX * 255); // Red from left to right
            UINT g = static_cast<UINT>(normY * 255); // Green from top to bottom
            UINT b = 255; // Constant blue component

            // Combine the color components into ARGB format
            colorData[y * width + x] = (255 << 24) | (b << 16) | (g << 8) | r; // ARGB
        }
    }

    // Specify the subresource data
    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = colorData.data(); // Pointer to the color data managed by std::vector
    initData.SysMemPitch = width * sizeof(UINT); // Pitch (bytes per row)
    initData.SysMemSlicePitch = 0;        // Not used for 2D textures

    // Update the texture with the color data
    m_deviceContext->UpdateSubresource(m_viewportTexture.Get(), 0, nullptr, colorData.data(), initData.SysMemPitch, 0);
}
