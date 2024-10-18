#include "imgui_impl.h"

#include "imgui_window.h"

cpu_raytracer::imgui_impl::imgui_impl(HWND hwnd, std::shared_ptr<d3dclass> d3d)
    : m_hwnd(hwnd), m_d3d(d3d)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable docking

    ImGui_ImplWin32_Init(m_hwnd);
    ImGui_ImplDX11_Init(m_d3d->get_device().Get(), m_d3d->get_device_context().Get());

    m_viewportWindow = std::make_shared<viewport_window>();
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

void cpu_raytracer::imgui_impl::render()
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

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
