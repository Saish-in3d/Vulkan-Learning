#include "Device.h"

VKFW::vulkancore::Device::Device(Sharedref<Instance> instance, Sharedref<WindowSurface> surface)
	:mInstance(instance), mSurface(surface)
{
	pickPhysicalDevice();
	initQueueFamilies(mPhysicalDevice);
	createLogicalDevice();

}

VKFW::vulkancore::Device::~Device()
{

	vkDestroyDevice(mDevice, nullptr);
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

void VKFW::vulkancore::Device::initQueueFamilies(VkPhysicalDevice device)
{
	uint32_t count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilyProperties(count);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &count, queueFamilyProperties.data());

	for (int i = 0; i < count; i++)
	{
		if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			mGraphicQueueFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, mSurface->GetSurface(), &presentSupport);
		if (presentSupport)
		{
			mPresentQueueFamily = i;
		}
	}

	if (!isQueFamilyComplete())
	{
		throw std::runtime_error("queue familes incomplete");
	}
}

bool VKFW::vulkancore::Device::isQueFamilyComplete()
{
	return mGraphicQueueFamily.has_value() && mPresentQueueFamily.has_value();
}

void VKFW::vulkancore::Device::createLogicalDevice()
{

	//getting queue family
	std::set<uint32_t> uniqueFamilies = {
		mGraphicQueueFamily.value(),
		mPresentQueueFamily.value()
	};

	float priotrity = 1.0f;
	std::vector<VkDeviceQueueCreateInfo>queueCreateInfos;
	for (uint32_t family : uniqueFamilies)
	{
		VkDeviceQueueCreateInfo createinfo{};
		createinfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		createinfo.queueFamilyIndex = family;
		createinfo.queueCount = 1;
		createinfo.pQueuePriorities = &priotrity;
		queueCreateInfos.push_back(createinfo);
	}

	VkPhysicalDeviceFeatures features{};
	features.samplerAnisotropy = VK_TRUE;

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &features;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mDevice))
	{
		throw std::runtime_error("failed to create logical device");
	}

	vkGetDeviceQueue(mDevice, mGraphicQueueFamily.value(), 0, &mGraphicQueue);
	vkGetDeviceQueue(mDevice, mPresentQueueFamily.value(), 0, &mPresentQueue);

}
