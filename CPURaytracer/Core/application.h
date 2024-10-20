#pragma once

#include <Windows.h>
#include <memory>
#include <vector>

namespace cpu_raytracer 
{
	class application
	{
	public:
		application();
		~application();
		const std::vector<BYTE>& frame(int width, int height, float deltaTime);
	private:
		void render(int width, int height);
	private:
		std::vector<BYTE> m_data;
	};
}
