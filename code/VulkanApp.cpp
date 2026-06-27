#include "VulkanApp.h"


namespace VKFW
{

	void VulkanApp::run()
	{
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();

	}

	void VulkanApp::initWindow()
	{
		window = MakeSharedRef<Platform::Window>(width, height);


	}

	void VKFW::VulkanApp::initVulkan()
	{
	}

	void VKFW::VulkanApp::mainLoop()
	{
		while (!window->shouldClose())
		{
			window->poolEvents();
		}
	}

	void VKFW::VulkanApp::cleanup()
	{
		window.reset();
	}
}
