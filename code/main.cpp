

#include "common.h"

int main()
{

	glm::vec3 testvec(1.0f, 2.0f, 3.0f);
	std::cout << glm::length(testvec) << std::endl;

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.apiVersion = VK_API_VERSION_1_4;
	appInfo.pApplicationName = "TestVulkan";

	VkInstanceCreateInfo createInfo{};

	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	VkInstance instance;
	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

	if(result != VK_SUCCESS)
	{
		std::cerr << "Failed to create Vulkan instance: " << result << std::endl;
		return -1;
	}
	std::cout << "Vulkan instance created successfully!" << std::endl;

	return 0;
}