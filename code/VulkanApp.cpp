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
		mwindow = MakeSharedRef<Platform::Window>(width, height);

		mwindow->setOnResize(
			[this](int w, int h )
			{
				this->onResize(w, h);

			});

	}

	void VKFW::VulkanApp::initVulkan()
	{

		mInstance = MakeSharedRef<vulkancore::Instance>(bool(false));
		mSurface = MakeSharedRef<vulkancore::WindowSurface>(mInstance,mwindow);

		auto testDevice = MakeSharedRef<vulkancore::Device>(mInstance,mSurface);
	}

	void VKFW::VulkanApp::mainLoop()
	{
		while (!mwindow->shouldClose())
		{
			mwindow->poolEvents();
		}
	}

	void VKFW::VulkanApp::cleanup()
	{
		mInstance.reset();
		mwindow.reset();
		mSurface.reset();
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
