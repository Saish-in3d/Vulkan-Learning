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

		[[nodiscard]] std::optional<uint32_t> GetGraphicQueueFamily()const { return mGraphicQueueFamily; }
		[[nodiscard]] std::optional<uint32_t> GetPresentQueueFamily()const { return mPresentQueueFamily; }
		

	private:

		void pickPhysicalDevice();

		void initQueueFamilies(VkPhysicalDevice device);
		bool isQueFamilyComplete();

		VkPhysicalDevice mPhysicalDevice{ nullptr };

		Sharedref<Instance> mInstance{ nullptr };
		Sharedref<WindowSurface> mSurface{ nullptr };

		std::optional<uint32_t> mGraphicQueueFamily;
		std::optional<uint32_t> mPresentQueueFamily;

		



		const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
	};

}