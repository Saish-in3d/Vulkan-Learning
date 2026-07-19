#include "WindowSurface.h"

namespace VKFW::vulkancore {
	WindowSurface::WindowSurface(Sharedref<Instance> instance, Sharedref<Platform::Window> window)
		:mInstance(instance)
	{

		if (glfwCreateWindowSurface(instance->getInstance(), window->GetWindow(), nullptr, &mSurface) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create glfw window surface");
		}
		
	}
	WindowSurface::~WindowSurface()
	{

		if (mSurface != VK_NULL_HANDLE)
		{
			vkDestroySurfaceKHR(mInstance->getInstance(), mSurface, nullptr);
		}

	}
}