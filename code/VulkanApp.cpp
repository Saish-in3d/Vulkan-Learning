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
			drawFrame();
		}
		vkDeviceWaitIdle(mDevice->getDevice());
	}

	void VKFW::VulkanApp::cleanup()
	{
		mRenderPass.reset();    
		mSwapChain.reset();
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

	void VulkanApp::drawFrame()
	{
		uint32_t imageIndex = 0;

		vkAcquireNextImageKHR(mDevice->getDevice(), mSwapChain->getSwapChain(), UINT64_MAX, VK_NULL_HANDLE, VK_NULL_HANDLE, &imageIndex);
		VkCommandBufferAllocateInfo allocinfo{};
		allocinfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocinfo.commandPool = mCommandPool->getCommandPool();
		allocinfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocinfo.commandBufferCount = 1;
		//std::cout << "Acquire: " << imageIndex << "\n";
		VkCommandBuffer cmd;
		vkAllocateCommandBuffers(mDevice->getDevice(), &allocinfo, &cmd);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		vkBeginCommandBuffer(cmd, &beginInfo);

		VkClearValue clearColor{};
		clearColor.color = { 0.0f, 0.2f,0.4f,1.0f };
		VkRenderPassBeginInfo rpInfo{};
		rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		rpInfo.renderPass = mRenderPass->getRenderPass();
		rpInfo.framebuffer = mSwapChain->getSwapChainFramebuffers()[imageIndex];
		rpInfo.renderArea.offset = { 0,0 };
		rpInfo.renderArea.extent = mSwapChain->getSwapChainExtent();
		rpInfo.clearValueCount = 1;
		rpInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);
		//doing nothing here
		vkCmdEndRenderPass(cmd);
		vkEndCommandBuffer(cmd);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmd;

		vkQueueSubmit(mDevice->getGraphicQueue(), 1, &submitInfo, VK_NULL_HANDLE);

		vkQueueWaitIdle(mDevice->getGraphicQueue());

		vkFreeCommandBuffers(mDevice->getDevice(), mCommandPool->getCommandPool(), 1, &cmd);

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.swapchainCount = 1;
		VkSwapchainKHR sc = mSwapChain->getSwapChain();
		presentInfo.pSwapchains = &sc;
		presentInfo.pImageIndices = &imageIndex;
		
		vkQueuePresentKHR(mDevice->getPresentQueue(), &presentInfo);
		
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
