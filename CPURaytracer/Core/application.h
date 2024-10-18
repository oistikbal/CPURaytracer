#pragma once

#include <Windows.h>
#include <memory>

namespace cpu_raytracer 
{
	class application
	{
	public:
		application();
		~application();
		bool frame(float deltaTime);
		void resize(int width, int height);
	private:
		bool render(float);
	private:
	};
}
