#include "VulkanApp.h"

namespace VKFW
{

	void VKFW::VulkanApp::run()
	{
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();

	}

	void VKFW::VulkanApp::initWindow()
	{
		if(!glfwInit())
		{
			throw std::runtime_error("Failed to initialize GLFW");
		}
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Tell GLFW not to create an OpenGL context
		glfwCreateWindow(width, height, "Vulkan Window", nullptr, nullptr);


		if (!window)
		{
			glfwTerminate();
			throw std::runtime_error("glfwcreatewindowfailed");
		}


	}

	void VKFW::VulkanApp::initVulkan()
	{
	}

	void VKFW::VulkanApp::mainLoop()
	{
		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
		}
	}

	void VKFW::VulkanApp::cleanup()
	{
		if (window)
		{
			glfwDestroyWindow(window);
			glfwTerminate();
		}
	}
}
