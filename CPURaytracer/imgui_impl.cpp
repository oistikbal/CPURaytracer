#include "imgui_impl.h"

#include "imgui_window.h"

cpu_raytracer::imgui_impl::imgui_impl(HWND hwnd, std::shared_ptr<d3dclass> d3d)
    : m_hwnd(hwnd), m_d3d(d3d)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; 

    ImGuiStyle& style = ImGui::GetStyle();

    // Set colors
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f); // Text color
    colors[ImGuiCol_TextDisabled] = ImVec4(0.5f, 0.5f, 0.5f, 1.00f);  // Disabled text
    colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f); // Background color
    colors[ImGuiCol_ChildBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f); // Child background
    colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f); // Popup background
    colors[ImGuiCol_Border] = ImVec4(0.5f, 0.5f, 0.5f, 0.5f);     // Border color
    colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);     // Border shadow
    colors[ImGuiCol_FrameBg] = ImVec4(0.23f, 0.23f, 0.23f, 1.00f); // Frame background
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f); // Frame background on hover
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f); // Frame background when active
    colors[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f); // Title background
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f); // Title background active
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f); // Collapsed title background
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f); // Menu bar background
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f); // Scrollbar background
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.25f, 0.45f, 0.85f, 1.00f); // Scrollbar grab color
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.30f, 0.55f, 0.95f, 1.00f); // Scrollbar grab hover color
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.20f, 0.45f, 0.80f, 1.00f); // Scrollbar grab active color
    colors[ImGuiCol_Button] = ImVec4(0.25f, 0.45f, 0.85f, 1.00f); // Button color
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.55f, 0.95f, 1.00f); // Button hover color
    colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.45f, 0.80f, 1.00f); // Button active color
    colors[ImGuiCol_Header] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f); // Header color
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.45f, 0.85f, 1.00f); // Header hover color
    colors[ImGuiCol_HeaderActive] = ImVec4(0.35f, 0.55f, 0.95f, 1.00f); // Header active color
    colors[ImGuiCol_Separator] = ImVec4(0.5f, 0.5f, 0.5f, 0.5f);     // Separator color
    colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f); // Tab color
    colors[ImGuiCol_TabHovered] = ImVec4(0.20f, 0.45f, 0.85f, 1.00f); // Tab hover color
    colors[ImGuiCol_TabActive] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f); // Active tab color
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);     // Drag and drop target color
    colors[ImGuiCol_NavHighlight] = ImVec4(0.25f, 0.45f, 0.85f, 1.00f); // Navigation highlight color
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 0.00f, 0.70f); // Navigation windowing highlight color
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.20f); // Navigation windowing dim background

    // Set style settings
    style.WindowRounding = 0.0f;      // No rounding for windows
    style.FrameRounding = 2.0f;       // Rounding for frame elements
    style.ScrollbarRounding = 4.0f;   // Scrollbar rounding
    style.GrabRounding = 2.0f;        // Grab rounding for sliders
    style.TabRounding = 2.0f;         // Rounding for tabs

    // Padding settings
    style.ItemSpacing = ImVec2(6, 6); // Space between items
    style.FramePadding = ImVec2(4, 4); // Padding within frames
    style.WindowPadding = ImVec2(8, 8); // Padding within windows

    // Border size
    style.FrameBorderSize = 1.0f;     // Border size for frames
    style.WindowBorderSize = 1.0f;    // Border size for windows

    // Minimum sizes
    style.WindowMinSize = ImVec2(50, 50); // Minimum window size

    // Other style settings
    style.Alpha = 1.0f;                // Overall transparency
    style.TouchExtraPadding = ImVec2(0, 0); // Touch extra padding
    style.ItemInnerSpacing = ImVec2(5, 5); // Inner spacing for items
    style.IndentSpacing = 21.0f;       // Indent spacing for tree nodes
    style.ColumnsMinSpacing = 6.0f;    // Minimum spacing between columns
    style.ScrollbarSize = 16.0f;       // Size of the scrollbar

    ImGui_ImplWin32_Init(m_hwnd);
    ImGui_ImplDX11_Init(m_d3d->get_device().Get(), m_d3d->get_device_context().Get());

    m_viewportWindow = std::make_shared<viewport_window>();
    m_settingsWindow = std::make_shared<settings_window>(m_viewportWindow);
}

cpu_raytracer::imgui_impl::~imgui_impl()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void cpu_raytracer::imgui_impl::begin_frame()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void cpu_raytracer::imgui_impl::render(float deltaTime)
{
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            ImGui::MenuItem("Empty");
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Assets")) {
            ImGui::MenuItem("Empty");
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Windows")) {
            if (ImGui::MenuItem("Viewport"))
                m_viewportWindow->set_open(true);
            if (ImGui::MenuItem("Settings"))
                m_settingsWindow->set_open(true);
            ImGui::EndMenu();
        }

        //ImGui::SameLine(ImGui::GetWindowWidth() - 80); // Adjust spacing to right
        //if (ImGui::Button("H")) {
        //}
        //ImGui::SameLine();
        //if (ImGui::Button("M")) {
        //}
        //ImGui::SameLine();
        //if (ImGui::Button("X")) {
        //}

        ImGui::EndMainMenuBar();
    }

    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID);

    m_viewportWindow->render(m_d3d->get_viewport_view());
    m_settingsWindow->render(deltaTime);

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
