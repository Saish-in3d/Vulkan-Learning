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

		mSwapChain = MakeSharedRef<vulkancore::SwapChain>(mDevice,mwindow,mSurface);

		mRenderPass = createRenderPass();
		mSwapChain->createFrameBuffers(mRenderPass);

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

	Sharedref<vulkancore::RenderPass> VulkanApp::createRenderPass()
	{
		auto rp = MakeSharedRef<vulkancore::RenderPass>(mDevice);
		VkAttachmentDescription color{};
		color.format = mSwapChain->getSwapChainImageFormat();
		color.samples = VK_SAMPLE_COUNT_1_BIT;
		color.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		color.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		color.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		rp->addAttachment(color);

		VkAttachmentReference colorRef{};
		colorRef.attachment = 0;
		colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		vulkancore::SubPass subpass;
		subpass.addColorAttachmentReference(colorRef);
		rp->addSubpass(subpass);

		VkSubpassDependency dep1{};
		dep1.srcSubpass = VK_SUBPASS_EXTERNAL;
		dep1.dstSubpass = 0;
		dep1.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dep1.srcAccessMask = 0;
		dep1.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dep1.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		rp->addDependency(dep1);

		rp->buildRenderPass();


		return rp;
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
