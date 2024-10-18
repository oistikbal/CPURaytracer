#include <Windows.h>
#include <tchar.h>
#include <memory>

#include "system.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    std::unique_ptr<cpu_raytracer::system> app = std::make_unique<cpu_raytracer::system>();
    app->run();
    return 0;
}

