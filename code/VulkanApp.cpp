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

		mInstance = MakeSharedRef<vulkancore::Instance>(bool(true));
		mSurface = MakeSharedRef<vulkancore::WindowSurface>(mInstance,mwindow);

		mDevice = MakeSharedRef<vulkancore::Device>(mInstance,mSurface);
		mCommandPool = MakeSharedRef<vulkancore::CommandPool>(mDevice);

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
		mCommandPool.reset();
		mDevice.reset();
		mSurface.reset();
		mInstance.reset();
		mwindow.reset();
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
