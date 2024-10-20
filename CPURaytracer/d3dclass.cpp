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

    D3D11_VIEWPORT viewport;
    viewport.Width = static_cast<FLOAT>(renderWidth);
    viewport.Height = static_cast<FLOAT>(renderHeight);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    m_deviceContext->RSSetViewports(1, &viewport);

    resize(renderWidth, renderHeight, 8, 8);
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

void cpu_raytracer::d3dclass::resize(int width, int height, int textureWidth, int textureHeight)
{
    destroy_render_target();
    m_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    create_render_target();
    resize_window_texture(textureWidth, textureHeight);
}

void cpu_raytracer::d3dclass::resize_window_texture(int width, int height)
{
    static int _width = 0;
    static int _height = 0;

    if (width < 8)
        width = 8;

    if (height < 8)
        height = 8;

    if (_width == width && _height == height)
        return;

    if (m_viewportTexture)
        m_viewportTexture.Reset();
    if (m_viewportView)
        m_viewportView.Reset();

    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = width;         // Width of the texture
    textureDesc.Height = height;       // Height of the texture
    textureDesc.MipLevels = 1;         // Number of mipmap levels
    textureDesc.ArraySize = 1;         // Number of textures in the array
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Texture format
    textureDesc.SampleDesc.Count = 1;  // No multi-sampling
    textureDesc.Usage = D3D11_USAGE_DYNAMIC; // Usage flag
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // Bind the texture for shader use
    textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT hr = m_device->CreateTexture2D(&textureDesc, nullptr, m_viewportTexture.GetAddressOf());

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;

    m_device->CreateShaderResourceView(m_viewportTexture.Get(), &srvDesc, m_viewportView.GetAddressOf());

    _width = width;
    _height = height;
}

void cpu_raytracer::d3dclass::upload_texture(const std::vector<BYTE>& data, int width, int height)
{

    D3D11_MAPPED_SUBRESOURCE mappedResource;

    // Map the texture for writing
    m_deviceContext->Map(m_viewportTexture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    // Calculate the pointer to the mapped resource
    BYTE* pData = static_cast<BYTE*>(mappedResource.pData);

    // Copy each row of the texture
    for (UINT y = 0; y < height; ++y)
    {
        // Copy the row with correct pitch handling
        memcpy(
            &pData[y * mappedResource.RowPitch],         // Destination row pointer with correct pitch
            &data[y * width * 4],                        // Source row in 'data' (4 bytes per pixel: RGBA)
            width * 4                                    // Copy width (4 bytes per pixel)
        );
    }

    // Unmap the texture to apply changes
    m_deviceContext->Unmap(m_viewportTexture.Get(), 0);
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
}

void cpu_raytracer::d3dclass::create_render_target()
{
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
    m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf());
}