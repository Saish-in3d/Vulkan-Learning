#include "Device.h"

VKFW::vulkancore::Device::Device(Sharedref<Instance> instance, Sharedref<WindowSurface> surface)
	:mInstance(instance), mSurface(surface)
{
	pickPhysicalDevice();


}

VKFW::vulkancore::Device::~Device()
{


}

void VKFW::vulkancore::Device::pickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(mInstance->getInstance(), &deviceCount, nullptr);
	if (deviceCount == 0)
	{
		throw std::runtime_error("No devices/ vulkan capable found");
	}

	std::vector<VkPhysicalDevice>devices(deviceCount);
	vkEnumeratePhysicalDevices(mInstance->getInstance(), &deviceCount, devices.data());

	for (const auto& device : devices)
	{
		VkPhysicalDeviceProperties props;
		vkGetPhysicalDeviceProperties(device, &props);

		VkPhysicalDeviceFeatures features;
		vkGetPhysicalDeviceFeatures(device, &features);

		//chacking/Adding swapchain queue support
		uint32_t extCount = 0;

		vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, nullptr);
		std::vector<VkExtensionProperties> extProperties(extCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, extProperties.data());

		bool hasSwapchain = false;
		for (const auto& ext : extProperties)
		{
			if (std::string(ext.extensionName) == VK_KHR_SWAPCHAIN_EXTENSION_NAME)
			{
				hasSwapchain = true;
				break;
			}
		}
		if (!hasSwapchain)
		{
			continue;
		}

		//chacking/addding graphics and present queue families
		uint32_t quefamilycount = 0;

		vkGetPhysicalDeviceQueueFamilyProperties(device, &quefamilycount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilyProperties(quefamilycount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &quefamilycount, queueFamilyProperties.data());

		bool hasGraphics = false;
		bool hasPresent = false;

		for (int i = 0; i < quefamilycount; i++)
		{
			if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				hasGraphics = true;
				VkBool32 present = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(device, i, mSurface->GetSurface(), &present);
				if (present)
				{
					hasPresent = true;
				}
			}
		}
		if (hasGraphics && hasPresent)
		{
			mPhysicalDevice = device;
			std::cout << "GPU: " << props.deviceName << std::endl;
			return;
		}




		//Swapchain support extensions
	}

	throw std::runtime_error("no suitable GPU found");

}
