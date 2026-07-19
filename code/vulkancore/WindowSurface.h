#pragma once
#include "vk_common.h"
#include "instance.h"
#include "../platform/Window.h"

namespace VKFW::vulkancore
{

	class WindowSurface
	{
	public:
		 
		WindowSurface(Sharedref<Instance> instance, Sharedref<Platform::Window> window);
		~WindowSurface();

		VkSurfaceKHR GetSurface() const { return mSurface; }

		VkInstance GetInstance() const { return mInstance->getInstance(); }

	private:

		VkSurfaceKHR mSurface{ VK_NULL_HANDLE };
		Sharedref<Instance> mInstance{ nullptr };


	};

}