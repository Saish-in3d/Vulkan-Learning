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

		window->setOnResize(
			[this](int w, int h )
			{
				this->onResize(w, h);

			});

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


	void VulkanApp::onResize(int w, int h)
	{

		width = w;
		height = h;
		mFrameBufferResized = true;
		std::cout << "Resized" << w << "x" << h << "y"<<std::endl;
		//Vulkan implementation on resize

	}
}
