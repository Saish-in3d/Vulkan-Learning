#include "instance.h"

namespace VKFW::vulkancore {


	Instance::Instance(bool enableValidationLayer)
	{
		mEnableValidationLayer =  enableValidationLayer;

		uint32_t supportedVersion = VK_API_VERSION_1_0;
		vkEnumerateInstanceVersion(&supportedVersion);
		if (supportedVersion < VK_API_VERSION_1_4)
		{
			throw std::runtime_error("Error: Vulkan 1.4 not supported on this system");
		}

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "vulkanframework";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "NO ENGINE";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_4;

		auto extensions = getRequiredExtensions();
		
		
		VkInstanceCreateInfo ci{};
		ci.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		ci.pApplicationInfo = &appInfo;
		ci.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		ci.ppEnabledExtensionNames = extensions.data();

		if (vkCreateInstance(&ci, nullptr, &mInstance) != VK_SUCCESS)
		{
			throw std::runtime_error("Error::Faied to create instance");
		}
		

	}

	Instance::~Instance()
	{

		vkDestroyInstance(mInstance, nullptr);
	}

	std::vector<const char*> Instance::getRequiredExtensions()
	{

		uint32_t glfwExtCount = 0;
		const char** glfwExts = glfwGetRequiredInstanceExtensions(&glfwExtCount);

		std::vector<const char*> extensions(glfwExts, glfwExts + glfwExtCount);

		return extensions;
	}

	void Instance::printAvailableExtensions()
	{
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		std::cout << "Available extensions" << std::endl;

		for(const auto& ext : extensions)
		{
			std::cout << " " << ext.extensionName << std::endl;
		}

	}

}