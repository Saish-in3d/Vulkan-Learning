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

		mRenderPass = factories::RenderPassFactory::CreateSimpleRenderPass(mDevice,mSwapChain->getSwapChainImageFormat());
		mSwapChain->createFrameBuffers(mRenderPass);

		mPipeline = createPipeline();
		recordCommandBuffers();

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


	Sharedref<vulkancore::Pipeline> VulkanApp::createPipeline()
	{
		auto pipeline = MakeSharedRef<vulkancore::Pipeline>(mDevice, mRenderPass);

		// === Shaders ===
		// spv files compiled by CMake and copied to exe/spv/
		// entry point is "main" — Slang renames vertMain/fragMain to main in SPIR-V
		const auto shaderDir = exeDir() / "spv";

		auto vertShader = MakeSharedRef<vulkancore::Shader>(
			mDevice, (shaderDir / "test_vert.spv").string(),
			VK_SHADER_STAGE_VERTEX_BIT, "main");

		auto fragShader = MakeSharedRef<vulkancore::Shader>(
			mDevice, (shaderDir / "test_frag.spv").string(),
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
		
		VkCommandBuffer cmd = mCommandBuffers[imageIndex]->getCommandBuffer();

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmd;

		vkQueueSubmit(mDevice->getGraphicQueue(), 1, &submitInfo, VK_NULL_HANDLE);

		vkQueueWaitIdle(mDevice->getGraphicQueue());

		//vkFreeCommandBuffers(mDevice->getDevice(), mCommandPool->getCommandPool(), 1, &cmd);

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.swapchainCount = 1;
		VkSwapchainKHR sc = mSwapChain->getSwapChain();
		presentInfo.pSwapchains = &sc;
		presentInfo.pImageIndices = &imageIndex;
		
		vkQueuePresentKHR(mDevice->getPresentQueue(), &presentInfo);
		
	}

	void VulkanApp::recordCommandBuffers()
	{

		auto framebuffers = mSwapChain->getSwapChainFramebuffers();
		auto extent = mSwapChain->getSwapChainExtent();
		auto count = mSwapChain->getImageCount();

		mCommandBuffers.resize(count);
		for (uint32_t i = 0; i < count; i++)
		{
			mCommandBuffers[i] = MakeSharedRef<vulkancore::CommandBuffer>(mDevice, mCommandPool);

			//COMMANDbuffer recording starts here
			mCommandBuffers[i]->beginCommandBuffer();
			VkClearValue clearColor{};
			clearColor.color = { 0.0f, 0.2f,0.4f,1.0f };

			VkRenderPassBeginInfo rpInfo{};
			rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			rpInfo.renderPass = mRenderPass->getRenderPass();
			rpInfo.framebuffer = mSwapChain->getSwapChainFramebuffers()[i];
			rpInfo.renderArea.offset = { 0,0 };
			rpInfo.renderArea.extent = mSwapChain->getSwapChainExtent();
			rpInfo.clearValueCount = 1;
			rpInfo.pClearValues = &clearColor;

			mCommandBuffers[i]->beginRenderPass(rpInfo);

			//Binding graphics pipeline
			mCommandBuffers[i]->bindGraphicPipeline(mPipeline);

			VkViewport viewport{};
			viewport.x = 0.f;
			viewport.y = static_cast<float>(extent.height); //(fLIPING THE VIEWPORT as per vulkan)
			viewport.width = static_cast<float>(extent.width);
			viewport.height = -static_cast<float>(extent.height);
			viewport.minDepth = 0.f;
			viewport.maxDepth = 0.f;
			vkCmdSetViewport(mCommandBuffers[i]->getCommandBuffer(), 0, 1, &viewport);

			VkRect2D scissor{};
			scissor.offset = { 0,0 };
			scissor.extent = extent;

			vkCmdSetScissor(mCommandBuffers[i]->getCommandBuffer(), 0, 1, &scissor);

			mCommandBuffers[i]->draw(3);
			mCommandBuffers[i]->endRenderPass();
			mCommandBuffers[i]->endCommandBuffer();
		}
		

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
