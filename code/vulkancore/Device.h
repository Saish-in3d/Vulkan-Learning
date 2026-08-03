#pragma once
#include "vk_common.h"
#include "instance.h"
#include "windowSurface.h"

namespace VKFW::vulkancore
{

	class Device
	{
	public:

		Device(Sharedref<Instance> instance, Sharedref<WindowSurface> surface);
		~Device();

		[[nodiscard]] VkPhysicalDevice getPhysicalDevice() const { return mPhysicalDevice; } 
		

	private:

		void pickPhysicalDevice();

		VkPhysicalDevice mPhysicalDevice{ nullptr };

		Sharedref<Instance> mInstance{ nullptr };
		Sharedref<WindowSurface> mSurface{ nullptr };



		const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
	};

}