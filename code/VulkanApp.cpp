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

	Sharedref<vulkancore::Pipeline> VulkanApp::createPipeline()
	{
		auto pipeline = MakeSharedRef<vulkancore::Pipeline>(mDevice, mRenderPass);

		// === Shaders ===
		// spv files compiled by CMake and copied to exe/spvs/
		// entry point is "main" — Slang renames vertMain/fragMain to main in SPIR-V
		auto vertShader = MakeSharedRef<vulkancore::Shader>(
			mDevice, "spvs/test_vert.spv",
			VK_SHADER_STAGE_VERTEX_BIT, "main");
		auto fragShader = MakeSharedRef<vulkancore::Shader>(
			mDevice, "spvs/test_frag.spv",
			VK_SHADER_STAGE_FRAGMENT_BIT, "main");
		pipeline->setShaderGroup({ vertShader, fragShader });

		// === Chapter 1: VertexInput — empty (no VBO, hardcoded in shader) ===
		// TODO L11: add binding descriptions and attribute descriptions
		pipeline->mVertexInputState.sType =
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		pipeline->mVertexInputState.vertexBindingDescriptionCount = 0;
		pipeline->mVertexInputState.vertexAttributeDescriptionCount = 0;

		// === Chapter 2: InputAssembly — every 3 vertices form one triangle ===
		pipeline->mAssemblyState.sType =
			VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		pipeline->mAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		pipeline->mAssemblyState.primitiveRestartEnable = VK_FALSE;

		// === Chapter 3: Dynamic state + Viewport ===
		// viewport and scissor are dynamic — set per frame in CommandBuffer
		// means we don't bake resolution into the pipeline
		pipeline->mDynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		pipeline->mDynamicState.sType =
			VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		pipeline->mDynamicState.dynamicStateCount =
			(uint32_t)pipeline->mDynamicStates.size();
		pipeline->mDynamicState.pDynamicStates = pipeline->mDynamicStates.data();

		// count = 1, actual values set dynamically in cmd buffer
		pipeline->mViewportState.sType =
			VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		pipeline->mViewportState.viewportCount = 1;
		pipeline->mViewportState.scissorCount = 1;

		// === Chapter 4: Rasterizer ===
		pipeline->mRasterState.sType =
			VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		pipeline->mRasterState.depthClampEnable = VK_FALSE;
		pipeline->mRasterState.rasterizerDiscardEnable = VK_FALSE;
		pipeline->mRasterState.polygonMode = VK_POLYGON_MODE_FILL;
		pipeline->mRasterState.lineWidth = 1.0f;
		pipeline->mRasterState.cullMode = VK_CULL_MODE_BACK_BIT;
		pipeline->mRasterState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		pipeline->mRasterState.depthBiasEnable = VK_FALSE;

		// === Chapter 5: Multisample — disabled, Part 2 enables MSAA ===
		pipeline->mMultisampleState.sType =
			VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		pipeline->mMultisampleState.sampleShadingEnable = VK_FALSE;
		pipeline->mMultisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		// === Chapter 6: ColorBlend — no blending, output replaces ===
		// TODO L14: enable blending for transparent materials
		VkPipelineColorBlendAttachmentState blendAttachment{};
		blendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		blendAttachment.blendEnable = VK_FALSE;
		pipeline->pushBlendAttachment(blendAttachment);

		pipeline->mBlendState.sType =
			VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		pipeline->mBlendState.logicOpEnable = VK_FALSE;
		pipeline->mBlendState.attachmentCount =
			(uint32_t)pipeline->mBlendAttachmentStates.size();
		pipeline->mBlendState.pAttachments =
			pipeline->mBlendAttachmentStates.data();

		// === Chapter 7: DepthStencil — disabled, no depth buffer yet ===
		// TODO L13: enable depth test and depth write
		pipeline->mDepthStencilState.sType =
			VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		pipeline->mDepthStencilState.depthTestEnable = VK_FALSE;
		pipeline->mDepthStencilState.depthWriteEnable = VK_FALSE;

		// === Chapter 8: PipelineLayout — empty, no descriptor sets, no push constants ===
		// TODO L12: add descriptor set layouts for UBO
		pipeline->mPipelineLayoutInfo.sType =
			VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline->mPipelineLayoutInfo.setLayoutCount = 0;
		pipeline->mPipelineLayoutInfo.pushConstantRangeCount = 0;

		pipeline->build();
		return pipeline;
	}

	void VulkanApp::drawFrame()
	{
		uint32_t imageIndex = 0;

		vkAcquireNextImageKHR(mDevice->getDevice(), mSwapChain->getSwapChain(),
			UINT64_MAX, VK_NULL_HANDLE, VK_NULL_HANDLE, &imageIndex);
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
